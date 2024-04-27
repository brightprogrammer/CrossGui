/**
 * @file Surface.c
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
 * */

/* local includes */
#include "Surface.h"

#include "Vulkan.h"

/* crosswindow */
#include <Anvie/CrossWindow/Vulkan.h>
#include <Anvie/CrossWindow/Window.h>

/* surface private methods */
static inline Surface *surface_create_surface (Surface *surface, XwWindow *win);
static inline Surface *surface_create_swapchain (Surface *surface, XwWindow *win);
static inline Surface *surface_fetch_swapchain_images (Surface *surface);
static inline Surface *surface_create_swapchain_image_views (Surface *surface);
static inline Surface *surface_create_command_objects (Surface *surface);
static inline Surface *surface_create_renderpass (Surface *surface);
static inline Surface *surface_create_framebuffers (Surface *surface);
static inline Surface *surface_create_sync_objects (Surface *surface);

/**************************************************************************************************/
/******************************* SURFACE PUBLIC METHOD DEFINITIONS ********************************/
/**************************************************************************************************/

/**
 * @b Initialize given surface object with new surface data
 *    as a new surface for given window.
 *
 * @param surf Surface to be initiazed
 * @param win Window to create new surface for.
 *
 * @return @c surf on success.
 * @return @c Null otherwise.
 * */
Surface *surface_init (Surface *surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, Null, ERR_INVALID_ARGUMENTS);

    GOTO_HANDLER_IF (
        !surface_create_surface (surface, win) || !surface_create_swapchain (surface, win) ||
            !surface_fetch_swapchain_images (surface) ||
            !surface_create_swapchain_image_views (surface) ||
            !surface_create_command_objects (surface) || !surface_create_renderpass (surface) ||
            !surface_create_framebuffers (surface) || !surface_create_sync_objects (surface),
        INIT_FAILED,
        "Failed to initialize Surface object\n"
    );

    return surface;

INIT_FAILED:
    surface_deinit (surface);
    return Null;
}

/**
 * @b De-initialize but don't free the given surface object.
 *
 * @param surf
 * @param vk 
 *
 * @return @c surface on success.
 * @return Null otherwise.
 * */
Surface *surface_deinit (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    /* for shorter name */
    VkDevice device = vk.device.logical;

    /* if device was ever created then start destroying possibly created objects */
    if (device) {
        vkDeviceWaitIdle (device);

        if (surface->render_semaphore) {
            vkDestroySemaphore (device, surface->render_semaphore, Null);
        }
        if (surface->present_semaphore) {
            vkDestroySemaphore (device, surface->present_semaphore, Null);
        }
        if (surface->render_fence) {
            vkDestroyFence (device, surface->render_fence, Null);
        }

        if (surface->framebuffers) {
            for (Size s = 0; s < surface->swapchain_image_count; s++) {
                if (surface->framebuffers[s]) {
                    vkDestroyFramebuffer (device, surface->framebuffers[s], Null);
                }
            }
        }

        if (surface->render_pass) {
            vkDestroyRenderPass (device, surface->render_pass, Null);
        }

        if (surface->cmd_pool) {
            vkDestroyCommandPool (device, surface->cmd_pool, Null);
        }

        if (surface->swapchain_image_views) {
            for (Size s = 0; s < surface->swapchain_image_count; s++) {
                if (surface->swapchain_image_views[s]) {
                    vkDestroyImageView (device, surface->swapchain_image_views[s], Null);
                    surface->swapchain_image_views[s] = VK_NULL_HANDLE;
                }
            }
        }

        if (surface->swapchain) {
            vkDestroySwapchainKHR (device, surface->swapchain, Null);
        }
    }

    if (surface->surface) {
        vkDestroySurfaceKHR (vk.instance, surface->surface, Null);
    }

    /* all memory deallocations out of if statements, having their own checks */
    if (surface->framebuffers) {
        FREE (surface->framebuffers);
        surface->framebuffers = Null;
    }

    if (surface->swapchain_image_views) {
        FREE (surface->swapchain_image_views);
        surface->swapchain_image_views = Null;
    }

    if (surface->swapchain_images) {
        FREE (surface->swapchain_images);
        surface->swapchain_images = Null;
    }

    return surface;
}

/**
 * @b Recreate swapchain for this surface, by using old swapchain.
 *
 * @param surface 
 *
 * @return @c surface on success.
 * @return @c Null otherwise.
 * */
Surface *surface_recreate_swapchain (Surface *surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, Null, ERR_INVALID_ARGUMENTS);

    /* shorter name for vk.device.logical */
    VkDevice device = vk.device.logical;

    RETURN_VALUE_IF (
        !surface->swapchain_image_views || !surface->framebuffers,
        Null,
        "Swapchain recreate called but, previous images/views/framebuffers are invalid\n"
    );

    vkDeviceWaitIdle (device);

    vkDestroySemaphore (device, surface->present_semaphore, Null);
    vkDestroySemaphore (device, surface->render_semaphore, Null);
    vkDestroyFence (device, surface->render_fence, Null);

    vkDestroyCommandPool (device, surface->cmd_pool, Null);

    /* destroy image views and framebuffers because they need to be recreated */
    for (Size s = 0; s < surface->swapchain_image_count; s++) {
        vkDestroyImageView (device, surface->swapchain_image_views[s], Null);
        vkDestroyFramebuffer (device, surface->framebuffers[s], Null);

        surface->swapchain_image_views[s] = VK_NULL_HANDLE;
        surface->framebuffers[s]          = VK_NULL_HANDLE;
    }

    /* store handle of old swapchain */
    VkSwapchainKHR old_swapchain = surface->swapchain;

    /* create new swapchain */
    RETURN_VALUE_IF (
        !surface_create_swapchain (surface, win) || !surface_fetch_swapchain_images (surface) ||
            !surface_create_swapchain_image_views (surface) ||
            !surface_create_framebuffers (surface) || !surface_create_sync_objects (surface) ||
            !surface_create_command_objects (surface),
        Null,
        "Failed to recreate swapchain\n"
    );

    /* destroy old swapchain after recreation */
    vkDestroySwapchainKHR (device, old_swapchain, Null);

    return surface;
}

/**************************************************************************************************/
/******************************* SURFACE PRIVATE METHOD DEFINITIONS *******************************/
/**************************************************************************************************/

/**
 * @b Create VkSurfaceKHR for given @c Surface object.
 *
 * @param surface Where surface will be stored after creation.
 * @param vk For access to VkInstance handle.
 * @param win @c XwWindow object to create this surface for.
 *
 * @return @c surface on success.
 * @return @c Null otherwise.
 * */
static inline Surface *surface_create_surface (Surface *surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, Null, ERR_INVALID_ARGUMENTS);

    VkResult res = xw_window_create_vulkan_surface (win, vk.instance, &surface->surface);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        Null,
        "Failed to create Vulkan surface for given window. RET = %d\n",
        res
    );

    return surface;
}

/**
 * @b Create swapchain for given @c Surface object.
 *
 * @param surface 
 * @param vk.device
 * @param win
 *
 * @return @c Surface on success.
 * @return @c Null otherwise.
 * */
static inline Surface *surface_create_swapchain (Surface *surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, VK_NULL_HANDLE, ERR_INVALID_ARGUMENTS);

    VkDevice         device = vk.device.logical;
    VkPhysicalDevice gpu    = vk.device.physical;

    /* get surface capabilities */
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR (gpu, surface->surface, &capabilities);

    VkExtent2D image_extent;
    if (capabilities.currentExtent.width == UINT32_MAX) {
        XwWindowSize win_size = xw_window_get_size (win);
        image_extent          = (VkExtent2D) {.width = win_size.width, .height = win_size.height};
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

    /* get number of available present modes */
    Uint32   present_mode_count = 0;
    VkResult res                = vkGetPhysicalDeviceSurfacePresentModesKHR (
        gpu,
        surface->surface,
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
        surface->surface,
        &present_mode_count,
        present_modes
    );
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to get present modes. RET = %d\n", res);

    /* select the best present mode out of all available. */
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (Size s = 0; s < present_mode_count; s++) {
        if (present_modes[s] == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }

    /* get number of available surface formats */
    Uint32 surface_format_count = 0;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR (gpu, surface->surface, &surface_format_count, Null);
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
        surface->surface,
        &surface_format_count,
        surface_formats
    );
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to get surface formats. RET = %d\n", res);

    /* the first format is the best format for now */
    VkSurfaceFormatKHR surface_format = surface_formats[0];

    /* only graphics family will be using our images */
    const Uint32 queue_family_indices[] = {vk.device.graphics_queue.family_index};

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType         = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext         = Null,
        .flags         = 0,
        .surface       = surface->surface,
        .minImageCount = CLAMP (
            capabilities.minImageCount,
            capabilities.minImageCount + 1,
            capabilities.maxImageCount ? capabilities.maxImageCount : capabilities.minImageCount + 1
        ),
        .imageFormat           = surface_format.format,
        .imageColorSpace       = surface_format.colorSpace,
        .imageExtent           = image_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = ARRAY_SIZE (queue_family_indices),
        .pQueueFamilyIndices   = queue_family_indices,
        .preTransform   = capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode    = present_mode,
        .clipped        = VK_TRUE,
        .oldSwapchain   = surface->swapchain /* use old swapchain, will be destroyed after this */
    };

    res = vkCreateSwapchainKHR (device, &swapchain_create_info, Null, &surface->swapchain);
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Vulkan swapchain. RET = %d\n", res);

    /* store required data in surface after swapchin creation */
    surface->swapchain_image_format = surface_format.format;
    surface->swapchain_image_extent = image_extent;

    return surface;
}

/**
 * @b Get images from swapchain and store them in Surface object.
 *
 * @param surface @c Surface which contains the valid swapchain.
 *
 * @return @c surf on success.
 * @return @c Null otherwise.
 * */
static inline Surface *surface_fetch_swapchain_images (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* get number of swapchain images */
    surface->swapchain_image_count = 0;
    VkResult res =
        vkGetSwapchainImagesKHR (device, surface->swapchain, &surface->swapchain_image_count, Null);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        Null,
        "Failed to get number of swapchain images. RET = %d\n",
        res
    );

    /* create space for images */
    surface->swapchain_images =
        REALLOCATE (surface->swapchain_images, VkImage, surface->swapchain_image_count);
    RETURN_VALUE_IF (!surface->swapchain_images, Null, ERR_OUT_OF_MEMORY);

    /* get swapchain image handles s */
    res = vkGetSwapchainImagesKHR (
        device,
        surface->swapchain,
        &surface->swapchain_image_count,
        surface->swapchain_images
    );
    GOTO_HANDLER_IF (
        res != VK_SUCCESS,
        IMAGE_GET_FAILED,
        "Failed to get all swapchain image handles. RET = %d\n",
        res
    );

    return surface;

IMAGE_GET_FAILED:
    FREE (surface->swapchain_images);
    return Null;
}

/**
 * @b Create color attachments (image views) for swapchain images.
 *
 * @param surface
 * 
 * @return @c surface on success.
 * @return @c Null otherwise.
 * */
static inline Surface *surface_create_swapchain_image_views (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* Create space to store image views in surface object.
     * Reallocate will reuse the memory if it's already allocated */
    surface->swapchain_image_views =
        REALLOCATE (surface->swapchain_image_views, VkImageView, surface->swapchain_image_count);
    RETURN_VALUE_IF (!surface->swapchain_image_views, Null, ERR_OUT_OF_MEMORY);

    /* template for creating image views */
    VkImageViewCreateInfo image_view_create_info = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = Null,
        .flags    = 0,
        .image    = VK_NULL_HANDLE,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = surface->swapchain_image_format,
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
    for (Size s = 0; s < surface->swapchain_image_count; s++) {
        image_view_create_info.image = surface->swapchain_images[s];
        VkResult res                 = vkCreateImageView (
            device,
            &image_view_create_info,
            Null,
            &surface->swapchain_image_views[s]
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            IMAGE_VIEW_CREATE_FAILED,
            "Failed to create Image view. RET = %d\n",
            res
        );
    }

    return surface;

IMAGE_VIEW_CREATE_FAILED:
    for (Size s = 0; s < surface->swapchain_image_count; s++) {
        if (surface->swapchain_image_views[s]) {
            vkDestroyImageView (device, surface->swapchain_image_views[s], Null);
        }
    }
    FREE (surface->swapchain_image_views);
    surface->swapchain_image_views = Null;
    return Null;
}

/**
 * @b Create command pool and allocate command buffers here.
 *
 * @param surface
 *
 * @return @c surface on success.
 * @return @c Null otherwise
 * */
static inline Surface *surface_create_command_objects (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = Null,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vk.device.graphics_queue.family_index
    };

    /* create command pool */
    VkResult res =
        vkCreateCommandPool (device, &command_pool_create_info, Null, &surface->cmd_pool);
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Command Pool. RET = %d\n", res);

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = Null,
        .commandPool        = surface->cmd_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    /* allocate command buffer(s) */
    res = vkAllocateCommandBuffers (device, &command_buffer_allocate_info, &surface->cmd_buffer);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        Null,
        "Failed to allocate Command Buffers. RET = %d\n",
        res
    );

    return surface;
}

/**
 * @b Create a renderpass object for this surface.
 * */
static inline Surface *surface_create_renderpass (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    VkAttachmentDescription color_attachment = {
        .flags          = 0,
        .format         = surface->swapchain_image_format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentReference color_attachment_reference = {
        .attachment = 0, /* index of color attachment*/
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference subpass_attachment_references[] = {color_attachment_reference};

    VkSubpassDescription subpass = {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = Null,
        .colorAttachmentCount    = ARRAY_SIZE (subpass_attachment_references),
        .pColorAttachments       = subpass_attachment_references,
        .pResolveAttachments     = Null,
        .pDepthStencilAttachment = Null,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = Null
    };

    VkAttachmentDescription render_pass_attachments[] = {color_attachment};
    VkSubpassDescription    render_pass_subpasses[]   = {subpass};

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = Null,
        .flags           = 0,
        .attachmentCount = ARRAY_SIZE (render_pass_attachments),
        .pAttachments    = render_pass_attachments,
        .subpassCount    = ARRAY_SIZE (render_pass_subpasses),
        .pSubpasses      = render_pass_subpasses,
        .dependencyCount = 0,
        .pDependencies   = Null,
    };

    VkResult res =
        vkCreateRenderPass (device, &render_pass_create_info, Null, &surface->render_pass);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        Null,
        "Failed to create Vulkan Render Pass. RET = %d\n",
        res
    );

    return surface;
}

/**
 * @b Create framebuffers for the swapchain inside surface.
 *
 * @param surface 
 *
 * @return @c surface on Success 
 * @return @c Null otherwise.
 * */
static inline Surface *surface_create_framebuffers (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    surface->framebuffers =
        REALLOCATE (surface->framebuffers, VkFramebuffer, surface->swapchain_image_count);
    RETURN_VALUE_IF (!surface->framebuffers, Null, ERR_OUT_OF_MEMORY);

    VkFramebufferCreateInfo framebuffer_create_info = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = Null,
        .flags           = 0,
        .renderPass      = surface->render_pass,
        .attachmentCount = 1, /* only color attachment for now */
        .pAttachments    = Null,
        .width           = surface->swapchain_image_extent.width,
        .height          = surface->swapchain_image_extent.height,
        .layers          = 1
    };

    for (Size s = 0; s < surface->swapchain_image_count; s++) {
        framebuffer_create_info.pAttachments = &surface->swapchain_image_views[s];
        VkResult res =
            vkCreateFramebuffer (device, &framebuffer_create_info, Null, &surface->framebuffers[s]);
        RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create framebuffer. RET = %d\n", res);
    }

    return surface;
}

/**
 * @b Create synchronization objects like semaphores, fences, etc.... and store in surface.
 *
 * @param surface 
 *
 * @return @c surface on success.
 * @return @c Null otherwise,
 * */
static inline Surface *surface_create_sync_objects (Surface *surface) {
    RETURN_VALUE_IF (!surface, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkResult res = vkCreateFence (device, &fence_create_info, Null, &surface->render_fence);
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Fence. RET = %d\n", res);

    VkSemaphoreCreateInfo semaphore_create_info =
        {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = Null, .flags = 0};

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &surface->render_semaphore);
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Semaphore. RET = %d\n", res);

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &surface->present_semaphore);
    RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Semaphore. RET = %d\n", res);

    return surface;
}
