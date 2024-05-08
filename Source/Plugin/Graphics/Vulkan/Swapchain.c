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

NEW_VECTOR_TYPE (SwapchainReinitHandlerData, reinit_handler);

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

    /* create surface for the swapchain only if it's not already created */
    if (!swapchain->surface) {
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
                .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext            = Null,
                .flags            = 0,
                .surface          = swapchain->surface,
                .minImageCount    = min_image_count,
                .imageFormat      = surface_format.format,
                .imageColorSpace  = surface_format.colorSpace,
                .imageExtent      = image_extent,
                .imageArrayLayers = 1,
                /* image will be used for color attachment but also be used for clear image operations. 
                 * Load/Store operations are transfer operations */
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
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
            (VkExtent3D) {swapchain->image_extent.width, swapchain->image_extent.height, 1},
            VK_FORMAT_D32_SFLOAT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            vk.device.graphics_queue.family_index
        ),
        INIT_FAILED,
        "Failed to create swapchain depth image\n"
    );

    /* create reinit handler data vector with initially 4 entries */
    if (!swapchain->reinit_handlers) {
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
            RETURN_VALUE_IF (
                !handler->handler (handler->render_pass, swapchain),
                Null,
                "One of the render pass(es) failed to handle swapchain-reinit-event.\n"
            );
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
 * @b Change layout of image in swapchain from @c initial_layout to @c final_layout.
 *
 * This will create an image memory barrier and perform the transition
 * of image by recording the image transition command to provided command
 * buffer. This can be dispatched separately in a separate command buffer,
 * or can be incorporated into command buffers including other commands.
 *
 * @WARN : Don't pass @c final_layout same as @c initial_layout.
 *         That's considered as invalid argument and function will
 *         return @c Null.
 *
 * @NOTE: The transition happens only as soon as the given command buffer is 
 *        submitted. The function only records commands to transition image,
 *        and does, not actually perform the image transition in place.
 *
 * @param swapchain @c Swapchain which contains the image.
 * @param img_idx Index of image in @c Swapchain to change layout.
 * @param cmd @c VkCommandBuffer object with already @c vkBeginCommandBuffer
 *        called upon it.
 * @param initial_layout Initial layout of image. If not known, then just pass 
 *        @c VK_IMAGE_LAYOUT_UNDEFINED.
 * @param final_layout Final layout of image to be transitioned to.
 *
 * @return @c swapchain on success.
 * @return @c Null otherwise. 
 * */
Swapchain *swapchain_change_image_layout (
    Swapchain      *swapchain,
    Uint32          img_idx,
    VkCommandBuffer cmd,
    VkImageLayout   initial_layout,
    VkImageLayout   final_layout
) {
    RETURN_VALUE_IF (
        !swapchain || !cmd || final_layout == initial_layout,
        Null,
        ERR_INVALID_ARGUMENTS
    );
    RETURN_VALUE_IF (
        img_idx >= swapchain->image_count,
        Null,
        "Swapchain image index out of bounds\n"
    );

    /* this is used multiple times to just create and keep it constant */
    VkImageSubresourceRange image_subrange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
    };

    VkImageMemoryBarrier barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = Null,
        .srcAccessMask       = 0,
        .dstAccessMask       = 0,
        .oldLayout           = initial_layout,
        .newLayout           = final_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = swapchain->images[img_idx].image,
        .subresourceRange    = image_subrange,
    };

    vkCmdPipelineBarrier (
        cmd,                               /* command buffer */
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, /* src stage mask */
        VK_PIPELINE_STAGE_TRANSFER_BIT, /* dst stage mask, because image load/clear is a transfer operation */
        0,                              /* dependency flags */
        0,                              /* memory barrier count */
        Null,                           /* memory barriers */
        0,                              /* buffer memory barrier count */
        Null,                           /* buffer barriers */
        1,                              /* image memory barrier count */
        &barrier                        /* image memory barriers */
    );

    return swapchain;
}

/**
 * @b Clear image. 
 *
 * This only records commands to clear image. The image is cleared as soon
 * as the command buffer in which the commands were recorded is submitted.
 *
 * @param image @c DeviceImage to be cleared.
 * @param cmd @c VkCommandBuffer object with @c vkBeginCommandBuffer already
 *        called upon it.
 * @param clear_value @c VkClearValue to use to clear the images.
 *
 * @return @c image on success.
 * @return @c Null otherwise.
 * */
Swapchain *swapchain_clear_image (
    Swapchain        *swapchain,
    Uint32            img_idx,
    VkCommandBuffer   cmd,
    VkClearColorValue clear_value
) {
    RETURN_VALUE_IF (!swapchain || !cmd, Null, ERR_INVALID_ARGUMENTS);

    /* clearing image is a transfer operation
     * layout of image during clear operation */
    swapchain_change_image_layout (
        swapchain,
        img_idx,
        cmd,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    VkImageSubresourceRange image_subrange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
    };

    /* get image handle */
    VkImage image = swapchain->images[img_idx].image;

    /* clear image */
    vkCmdClearColorImage (
        cmd,                                  /* command buffer */
        image,                                /* handle of image to be cleared */
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, /* layout of image */
        &clear_value,                         /* clear color */
        1,                                    /* subresource count */
        &image_subrange                       /* subresource ranges */
    );

    /* Add another barrier and put the image in VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
     * final layout is for presenting to screen */
    swapchain_change_image_layout (
        swapchain,
        img_idx,
        cmd,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    return swapchain;
}
