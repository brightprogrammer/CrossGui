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
 * [27-04-2024] : brightprogrammer
 *      - Renamed file from Surface.c to Swapchain.c
 * */

/* crosswindow */
#include <Anvie/CrossWindow/Vulkan.h>
#include <Anvie/CrossWindow/Window.h>

/* local includes */
#include "RenderTarget.h"
#include "Swapchain.h"
#include "Vulkan.h"

/* libc includes */
#include <memory.h>

/**
 * @b Initialize given Swapchain object.
 *
 * @param surf Surface to be initiazed
 * @param win Window to create new surface for.
 *
 * @return @c swapchain on success.
 * @return @c Null otherwise.
 * */
Swapchain *swapchain_init (Swapchain *swapchain, VkSurfaceKHR surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, Null, ERR_INVALID_ARGUMENTS);

    VkDevice         device = vk.device.logical;
    VkPhysicalDevice gpu    = vk.device.physical;

    /* create swapchain */
    {
        /* get surface capabilities */
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR (gpu, surface, &capabilities);

        VkExtent2D image_extent;
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

        /* get number of available present modes */
        Uint32   present_mode_count = 0;
        VkResult res =
            vkGetPhysicalDeviceSurfacePresentModesKHR (gpu, surface, &present_mode_count, Null);
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
            surface,
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
        res = vkGetPhysicalDeviceSurfaceFormatsKHR (gpu, surface, &surface_format_count, Null);
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
            surface,
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
            .surface       = surface,
            .minImageCount = CLAMP (
                capabilities.minImageCount,
                capabilities.minImageCount + 1,
                capabilities.maxImageCount ? capabilities.maxImageCount :
                                             capabilities.minImageCount + 1
            ),
            .imageFormat           = surface_format.format,
            .imageColorSpace       = surface_format.colorSpace,
            .imageExtent           = image_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = ARRAY_SIZE (queue_family_indices),
            .pQueueFamilyIndices   = queue_family_indices,
            .preTransform =
                capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha =
                capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = present_mode,
            .clipped     = VK_TRUE,
            .oldSwapchain =
                swapchain->swapchain /* use old swapchain, will be destroyed after this */
        };

        res = vkCreateSwapchainKHR (device, &swapchain_create_info, Null, &swapchain->swapchain);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to create Vulkan swapchain. RET = %d\n",
            res
        );

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
    if (device) {
        vkDeviceWaitIdle (device);

        /* destroy render targets */
        if (swapchain->render_targets) {
            for (Size s = 0; s < swapchain->image_count; s++) {
                render_target_deinit (swapchain->render_targets + s);
            }

            /* make all entries invalid */
            memset (swapchain->render_targets, 0, sizeof (RenderTarget) * swapchain->image_count);
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
        }

        if (swapchain->cmd_pool) {
            vkDestroyCommandPool (device, swapchain->cmd_pool, Null);
        }

        if (swapchain->swapchain) {
            vkDestroySwapchainKHR (device, swapchain->swapchain, Null);
        }
    }

    /* all memory deallocations out of if statements, having their own checks */
    if (swapchain->render_targets) {
        FREE (swapchain->render_targets);
        swapchain->render_targets = Null;
    }

    if (swapchain->images) {
        FREE (swapchain->images);
        swapchain->images = Null;
    }

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
Swapchain *swapchain_reinit (Swapchain *swapchain, VkSurfaceKHR surface, XwWindow *win) {
    RETURN_VALUE_IF (!surface || !win, Null, ERR_INVALID_ARGUMENTS);

    /* shorter name for vk.device.logical */
    VkDevice device = vk.device.logical;

    if (device) {
        vkDeviceWaitIdle (device);

        /* destroy render targets */
        if (swapchain->render_targets) {
            for (Size s = 0; s < swapchain->image_count; s++) {
                render_target_deinit (swapchain->render_targets + s);
            }

            /* make all entries invalid */
            memset (swapchain->render_targets, 0, sizeof (RenderTarget) * swapchain->image_count);
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
        }

        /* Destroy command pool, 
         * This will automatically free allocated command buffers in render target as well. */
        if (swapchain->cmd_pool) {
            vkDestroyCommandPool (device, swapchain->cmd_pool, Null);
        }

        /* store handle of old swapchain */
        VkSwapchainKHR old_swapchain = swapchain->swapchain;

        /* create new swapchain */
        swapchain_init (swapchain, surface, win);

        /* destroy old swapchain after recreation */
        vkDestroySwapchainKHR (device, old_swapchain, Null);
    }

    return swapchain;
}
