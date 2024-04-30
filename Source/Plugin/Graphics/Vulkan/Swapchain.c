/**
 * @file Swapchain.c
 * @date Sun, 21st April 2024
 * @author Siddharth Mishra (admin@brightprogrammer.in)
 * @copyright Copyright 2024 Siddharth Mishra
 * @copyright Copyright 2024 Anvie Labs
 *
 * Copyright 2024 Siddharth Mishra, Anvie Labs
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ----------------------------------------------------------------------------------------------------
 *
 * CHANGELOG :
 * [01-05-2024] : brightprogrammer
 *      - All future changelogs are updated in README.md in same folder.
 * [27-04-2024] : brightprogrammer
 *      - Renamed file from Surface.c to Swapchain.c
 * */

/* crosswindow */
#include <Anvie/CrossWindow/Vulkan.h>
#include <Anvie/CrossWindow/Window.h>

/* crossgui/utils */
#include <Anvie/CrossGui/Utils/Vector.h>

/* local includes */
#include "Anvie/Common.h"
#include "Swapchain.h"
#include "Vulkan.h"

/* libc includes */
#include <memory.h>
#include <vulkan/vulkan_core.h>

/**************************************************************************************************/
/********************************** PRIVATE METHOD DECLARATIONS ***********************************/
/**************************************************************************************************/

static inline SwapchainSyncObjects *swapchain_sync_objects_init (SwapchainSyncObjects *sync);
static inline SwapchainSyncObjects *swapchain_sync_objects_deinit (SwapchainSyncObjects *sync);

NEW_VECTOR_TYPE (SwapchainReinitHandlerData, reinit_handler);

/**************************************************************************************************/
/*********************************** PUBLIC METHOD DEFINITIONS ************************************/
/**************************************************************************************************/

/**
 * @b Initialize given Swapchain object.
 *
 * @param surf Surface to be initiazed
 * @param win Window to create new surface for.
 *
 * @return @c swapchain on success.
 * @return @c Null otherwise.
 * */
Swapchain *swapchain_init (Swapchain *swapchain, XwWindow *win) {
    RETURN_VALUE_IF (!swapchain || !win, Null, ERR_INVALID_ARGUMENTS);

    VkDevice         device = vk.device.logical;
    VkPhysicalDevice gpu    = vk.device.physical;

    /* create surface for the swapchain */
    {
        VkResult res = xw_window_create_vulkan_surface (win, vk.instance, &swapchain->surface);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create Vulkan surface. RET = %d\n",
            res
        );
    }

    /* create swapchain */
    {
        /* get surface capabilities */
        VkExtent2D                 image_extent;
        Uint32                     min_image_count;
        VkSurfaceTransformFlagsKHR transform_flags;
        VkCompositeAlphaFlagsKHR   composite_alpha;
        {
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR (gpu, swapchain->surface, &capabilities);

            transform_flags =
                capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

            composite_alpha =
                capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

            min_image_count = CLAMP (
                capabilities.minImageCount,
                capabilities.minImageCount + 1,
                capabilities.maxImageCount ? capabilities.maxImageCount :
                                             capabilities.minImageCount + 1
            );

            if (capabilities.currentExtent.width == UINT32_MAX) {
                XwWindowSize win_size = xw_window_get_size (win);
                image_extent = (VkExtent2D) {.width = win_size.width, .height = win_size.height};
            } else {
                image_extent = (VkExtent2D
                ) {.width = CLAMP (
                       capabilities.currentExtent.width,
                       capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width
                   ),
                   .height = CLAMP (
                       capabilities.currentExtent.height,
                       capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height
                   )};
            }
        }

        /* select the best present mode out of all available. */
        /* fifo is guaranteed to be available */
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
        {
            /* get number of available present modes */
            Uint32   present_mode_count = 0;
            VkResult res                = vkGetPhysicalDeviceSurfacePresentModesKHR (
                gpu,
                swapchain->surface,
                &present_mode_count,
                Null
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to get number of present modes. RET = %d\n",
                res
            );

            /* get list of present modes */
            VkPresentModeKHR present_modes[present_mode_count];
            res = vkGetPhysicalDeviceSurfacePresentModesKHR (
                gpu,
                swapchain->surface,
                &present_mode_count,
                present_modes
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to get present modes. RET = %d\n",
                res
            );

            /* select a better present mode */
            for (Size s = 0; s < present_mode_count; s++) {
                if (present_modes[s] == VK_PRESENT_MODE_MAILBOX_KHR) {
                    present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
                }
            }
        }

        /* select a proper surface format */
        VkSurfaceFormatKHR surface_format;
        {
            /* get number of available surface formats */
            Uint32   surface_format_count = 0;
            VkResult res                  = vkGetPhysicalDeviceSurfaceFormatsKHR (
                gpu,
                swapchain->surface,
                &surface_format_count,
                Null
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to get number of surface formats. RET = %d\n",
                res
            );

            /* get list of surface formats */
            VkSurfaceFormatKHR surface_formats[surface_format_count];
            res = vkGetPhysicalDeviceSurfaceFormatsKHR (
                gpu,
                swapchain->surface,
                &surface_format_count,
                surface_formats
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to get surface formats. RET = %d\n",
                res
            );

            /* the first format is the best format for now */
            surface_format = surface_formats[0];
        }

        /* finally create the swapchain handle */
        {
            /* only graphics family will be using our images */
            const Uint32 queue_family_indices[] = {vk.device.graphics_queue.family_index};

            VkSwapchainCreateInfoKHR swapchain_create_info = {
                .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext                 = Null,
                .flags                 = 0,
                .surface               = swapchain->surface,
                .minImageCount         = min_image_count,
                .imageFormat           = surface_format.format,
                .imageColorSpace       = surface_format.colorSpace,
                .imageExtent           = image_extent,
                .imageArrayLayers      = 1,
                .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = ARRAY_SIZE (queue_family_indices),
                .pQueueFamilyIndices   = queue_family_indices,
                .preTransform          = transform_flags,
                .compositeAlpha        = composite_alpha,
                .presentMode           = present_mode,
                .clipped               = VK_TRUE,
                .oldSwapchain =
                    swapchain->swapchain /* use old swapchain, will be destroyed after this */
            };

            VkResult res =
                vkCreateSwapchainKHR (device, &swapchain_create_info, Null, &swapchain->swapchain);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to create Vulkan swapchain. RET = %d\n",
                res
            );
        }

        /* store required data in surface after swapchin creation */
        swapchain->image_format = surface_format.format;
        swapchain->image_extent = image_extent;
    }

    /* get swapchain images */
    {
        /* get number of swapchain images */
        swapchain->image_count = 0;
        VkResult res =
            vkGetSwapchainImagesKHR (device, swapchain->swapchain, &swapchain->image_count, Null);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to get number of swapchain images. RET = %d\n",
            res
        );

        /* get swapchain image handles */
        VkImage swapchain_images[swapchain->image_count];
        res = vkGetSwapchainImagesKHR (
            device,
            swapchain->swapchain,
            &swapchain->image_count,
            swapchain_images
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to get all swapchain image handles. RET = %d\n",
            res
        );

        /* create space for images */
        swapchain->images = REALLOCATE (swapchain->images, SwapchainImage, swapchain->image_count);
        GOTO_HANDLER_IF (!swapchain->images, INIT_FAILED, ERR_OUT_OF_MEMORY);

        /* store image handles */
        for (Size s = 0; s < swapchain->image_count; s++) {
            swapchain->images[s].image = swapchain_images[s];
        }
    }

    /* create swapchain image views */
    {
        /* template for creating image views */
        VkImageViewCreateInfo image_view_create_info = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = Null,
            .flags    = 0,
            .image    = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = swapchain->image_format,
            .components =
                {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange =
                {.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                             .layerCount     = 1,
                             .baseArrayLayer = 0,
                             .levelCount     = 1,
                             .baseMipLevel   = 0}
        };

        /* create image view handles corresponding to each image in swapchain */
        for (Size s = 0; s < swapchain->image_count; s++) {
            /* image handle to create image view for. */
            image_view_create_info.image = swapchain->images[s].image;

            /* create image view */
            VkResult res = vkCreateImageView (
                device,
                &image_view_create_info,
                Null,
                &swapchain->images[s].view
            );

            GOTO_HANDLER_IF (
                res != VK_SUCCESS,
                INIT_FAILED,
                "Failed to create image view. RET = %d\n",
                res
            );
        }
    }

    /* create depth image for this swapchain */
    GOTO_HANDLER_IF (
        !device_image_init (
            &swapchain->depth_image,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            swapchain->image_extent.width,
            swapchain->image_extent.height,
            VK_FORMAT_D32_SFLOAT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            vk.device.graphics_queue.family_index
        ),
        INIT_FAILED,
        "Failed to create swapchain depth image\n"
    );

    /* init sync objects */
    {
        swapchain->sync_objects = ALLOCATE (SwapchainSyncObjects, swapchain->image_count);
        GOTO_HANDLER_IF (!swapchain->sync_objects, INIT_FAILED, ERR_OUT_OF_MEMORY);

        for (Size s = 0; s < swapchain->image_count; s++) {
            GOTO_HANDLER_IF (
                !swapchain_sync_objects_init (swapchain->sync_objects + s),
                INIT_FAILED,
                "Failed to init sync objects for swapchain.\n"
            );
        }
    }

    /* create reinit handler data vector with initially 4 entries */
    {
        swapchain->reinit_handlers = reinit_handler_vector_create (
            4,                                  /* initial count */
            &swapchain->reinit_handler_capacity /* get capacity */
        );

        GOTO_HANDLER_IF (
            !swapchain->reinit_handlers,
            INIT_FAILED,
            "Failed to create vector to hold swapchain-reinit-event handlers\n"
        );
    }

    return swapchain;

INIT_FAILED:
    swapchain_deinit (swapchain);
    return Null;
}

/**
 * @b De-initialize but don't free the given swapchain object.
 *
 * @param swapchain 
 *
 * @return @c swapchain on success.
 * @return Null otherwise.
 * */
Swapchain *swapchain_deinit (Swapchain *swapchain) {
    RETURN_VALUE_IF (!swapchain, Null, ERR_INVALID_ARGUMENTS);

    /* for shorter name */
    VkDevice device = vk.device.logical;

    /* if device was ever created then start destroying possibly created objects */
    vkDeviceWaitIdle (device);

    /* deinit sync objects */
    if (swapchain->sync_objects) {
        for (Size s = 0; s < swapchain->image_count; s++) {
            swapchain_sync_objects_deinit (swapchain->sync_objects + s);
        }

        /* make contents invalid */
        memset (swapchain->sync_objects, 0, sizeof (SwapchainSyncObjects) * swapchain->image_count);
        FREE (swapchain->sync_objects);
    }

    /* destroy depth image for this swapchain */
    if (swapchain->depth_image.image) {
        device_image_deinit (&swapchain->depth_image);
    }

    /* destroy image views in swapchain image */
    if (swapchain->images) {
        for (Size s = 0; s < swapchain->image_count; s++) {
            if (swapchain->images[s].view) {
                vkDestroyImageView (device, swapchain->images[s].view, Null);
            }
        }

        /* make all entries invalid */
        memset (swapchain->images, 0, sizeof (SwapchainImage) * swapchain->image_count);
        FREE (swapchain->images);
    }

    /* destroy swapchain */
    if (swapchain->swapchain) {
        vkDestroySwapchainKHR (device, swapchain->swapchain, Null);
    }

    /* destroy surface */
    if (swapchain->surface) {
        vkDestroySurfaceKHR (vk.instance, swapchain->surface, Null);
    }

    /* free event hander data */
    if (swapchain->reinit_handlers) {
        memset (
            swapchain->reinit_handlers,
            0,
            sizeof (SwapchainReinitHandlerData) * swapchain->reinit_handler_count
        );

        reinit_handler_vector_destroy (swapchain->reinit_handlers);
    }

    /* set all fields to invalid state */
    memset (swapchain, 0, sizeof (Swapchain));

    return swapchain;
}

/**
 * @b Recreate swapchain for this surface, by using old swapchain.
 *
 * @param surface 
 *
 * @return @c surface on success.
 * @return @c Null otherwise.
 * */
Swapchain *swapchain_reinit (Swapchain *swapchain, XwWindow *win) {
    RETURN_VALUE_IF (!swapchain || !win, Null, ERR_INVALID_ARGUMENTS);

    /* shorter name for vk.device.logical */
    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    /* deinit sync objects */
    if (swapchain->sync_objects) {
        for (Size s = 0; s < swapchain->image_count; s++) {
            swapchain_sync_objects_deinit (swapchain->sync_objects + s);
        }
    }

    /* deinit depth image */
    device_image_deinit (&swapchain->depth_image);

    /* destroy image views in swapchain image */
    if (swapchain->images) {
        for (Size s = 0; s < swapchain->image_count; s++) {
            if (swapchain->images[s].view) {
                vkDestroyImageView (device, swapchain->images[s].view, Null);
            }
        }

        /* make all entries invalid */
        memset (swapchain->images, 0, sizeof (SwapchainImage) * swapchain->image_count);
    }

    /* store handle of old swapchain */
    VkSwapchainKHR old_swapchain = swapchain->swapchain;

    /* create new swapchain */
    swapchain_init (swapchain, win);

    /* destroy old swapchain after recreation */
    vkDestroySwapchainKHR (device, old_swapchain, Null);

    /* After recreating the swapchain completely, ask registered RenderPass objects
     * to reinit their RenderTargets */
    if (swapchain->reinit_handlers) {
        for (Size s = 0; s < swapchain->reinit_handler_count; s++) {
            SwapchainReinitHandlerData *handler = swapchain->reinit_handlers + s;
            handler->handler (handler->render_pass, swapchain);
        }
    }

    return swapchain;
}

/**
 * @b Called by @c RenderPass objects created using this @c Swapchain to handle swapchain reinit 
 *    events.
 *
 * @param swapchain @c Swapchain object to register to.
 * @param handler @c Pointer to handler method of reinit events.
 * @param render_pass @c RenderPass object that'll handle the reinit event.
 * */
Bool swapchain_register_reinit_handler (
    Swapchain             *swapchain,
    SwapchainReinitHandler handler,
    RenderPass            *render_pass
) {
    RETURN_VALUE_IF (!swapchain || !handler || !render_pass, False, ERR_INVALID_ARGUMENTS);

    /* resize/create array if required */
    if (swapchain->reinit_handler_count >= swapchain->reinit_handler_capacity) {
        RETURN_VALUE_IF (
            !reinit_handler_vector_resize (
                swapchain->reinit_handlers,          /* vector data */
                swapchain->reinit_handler_count,     /* current count */
                swapchain->reinit_handler_count + 1, /* new count I want */
                swapchain->reinit_handler_capacity,  /* current capacity */
                &swapchain->reinit_handler_capacity  /* get new capacity */
            ),
            False,
            "Failed to resize swapchain-reinit-event handler vector\n"
        );
    }

    /* insert handler data */
    swapchain->reinit_handlers[swapchain->reinit_handler_count++] =
        (SwapchainReinitHandlerData) {.handler = handler, .render_pass = render_pass};

    return True;
}

/**
 * @b Begin rendering of next frame.
 *
 * @param swapchain
 * @param win
 *
 * @return On success, returns the index of next @c RenderTarget object to be used.
 * @return -1 otherwise.
 * */
Uint32 swapchain_begin_frame (Swapchain *swapchain, XwWindow *win) {
    RETURN_VALUE_IF (!swapchain || !win, -1, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* get next image index */
    Uint32 next_image_index = -1;
    {
        /* shorter name for currently in-use sync object */
        SwapchainSyncObjects *sync = swapchain->sync_objects + swapchain->current_sync_object_index;

        /* wait for all gpu rendering to complete */
        VkFence fences[] = {sync->render_fence};

        VkResult res = vkWaitForFences (device, ARRAY_SIZE (fences), fences, True, 1e9);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            -1,
            "Timeout (1s) while waiting for fences. RET = %d\n",
            res
        );

        /* get next image index */
        {
            res = vkAcquireNextImageKHR (
                device,
                swapchain->swapchain,
                1e9, /* 1e9 ns = 1 s */
                sync->present_semaphore,
                Null,
                &next_image_index
            );

            /* recoverable error cases */
            if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
                RETURN_VALUE_IF (
                    !swapchain_reinit (swapchain, win),
                    -1,
                    "Failed to recreate swapchain\n"
                );
                /* to make it continue if we succeed. */
                res = VK_SUCCESS;
            }

            /* irrecoverable */
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                -1,
                "Failed to get next image index from swapchain. RET = %d\n",
                res
            );
        }

        /* need to reset fence before we use it again */
        res = vkResetFences (device, ARRAY_SIZE (fences), fences);
        RETURN_VALUE_IF (res != VK_SUCCESS, -1, "Failed to reset fences. RET = %d\n", res);
    }

    return next_image_index;
}

/**
 * @b Submit given commands to queue and queue the current image to be presented
 *    to screen.
 *
 * @param swapchain
 * @param win
 * @param cmd Command buffer containing recorded commands.
 * @param image_index Index of image for which render commands are recorded.
 *
 * @return True on success.
 * @return False otherwise.
 * */
Bool swapchain_end_frame (
    Swapchain      *swapchain,
    XwWindow       *win,
    VkCommandBuffer cmd,
    Uint32          image_index
) {
    RETURN_VALUE_IF (
        !swapchain || !win || !cmd || (image_index == (Uint32)-1),
        False,
        ERR_INVALID_ARGUMENTS
    );

    SwapchainSyncObjects *sync = swapchain->sync_objects + swapchain->current_sync_object_index;

    /* submit for rendering */
    {
        /* wait when rendered image is being presented */
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit_info = {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = Null,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &sync->present_semaphore,
            .pWaitDstStageMask    = &wait_stage,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &sync->render_semaphore,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd
        };

        VkResult res =
            vkQueueSubmit (vk.device.graphics_queue.handle, 1, &submit_info, sync->render_fence);

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            False,
            "Failed to submit command buffers for execution. RET = %d\n",
            res
        );
    }

    /* submit for presentation to surface */
    {
        VkPresentInfoKHR present_info = {
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = Null,
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain->swapchain,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = (VkSemaphore[]) {sync->render_semaphore},
            .pImageIndices      = (Uint32[]) {image_index}
        };

        VkResult res = vkQueuePresentKHR (vk.device.graphics_queue.handle, &present_info);

        if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
            RETURN_VALUE_IF (
                !swapchain_reinit (swapchain, win),
                False,
                "Failed to recreate swapchain\n"
            );
            res = VK_SUCCESS; /* so that next check does not fail */
        }

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            -1,
            "Failed to present rendered images to surface. RET = %d\n",
            res
        );
    }

    /* change sync object */
    swapchain->current_sync_object_index =
        (swapchain->current_sync_object_index + 1) % swapchain->image_count;

    return True;
}

/**************************************************************************************************/
/*********************************** PRIVATE METHOD DEFINITIONS ***********************************/
/**************************************************************************************************/

/**
 * @b Initialize given @c SwapchainSyncObjects.
 *
 * @param sync.
 *
 * @return @c sync on success.
 * @return @c Null otherwise.
 * */
static inline SwapchainSyncObjects *swapchain_sync_objects_init (SwapchainSyncObjects *sync) {
    RETURN_VALUE_IF (!sync, Null, ERR_INVALID_ARGUMENTS);

    VkDevice          device            = vk.device.logical;
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkResult res = vkCreateFence (device, &fence_create_info, Null, &sync->render_fence);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Fence. RET = %d\n", res);

    VkSemaphoreCreateInfo semaphore_create_info =
        {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = Null, .flags = 0};

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &sync->render_semaphore);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Semaphore. RET = %d\n", res);

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &sync->present_semaphore);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Semaphore. RET = %d\n", res);

    return sync;

INIT_FAILED:
    swapchain_sync_objects_deinit (sync);
    return Null;
}

/**
 * @b De-initialize given @c SwapchainSyncObjects.
 *
 * @param sync.
 *
 * @return @c sync on success.
 * @return @c Null otherwise.
 * */
static inline SwapchainSyncObjects *swapchain_sync_objects_deinit (SwapchainSyncObjects *sync) {
    RETURN_VALUE_IF (!sync, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    if (sync->present_semaphore) {
        vkDestroySemaphore (device, sync->present_semaphore, Null);
    }
    if (sync->render_semaphore) {
        vkDestroySemaphore (device, sync->render_semaphore, Null);
    }
    if (sync->render_fence) {
        vkDestroyFence (device, sync->render_fence, Null);
    }

    /* restore to invalid state */
    memset (sync, 0, sizeof (SwapchainSyncObjects));

    return sync;
}
