/**
 * @file Swapchain.h
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
 *      - Renamed file from Surface.h to Swapchain.h
 *      - Removed everything from Surface object outside of it, and moved the VkSurfaceKHR handle 
 *        to XuiGraphicsContext object.
 *      - New objects created :
 *        - RenderTargetSyncObjects
 *        - RendetTarget 
 *        - Swapchain
 * */

#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SWAPCHAIN_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SWAPCHAIN_H

#include <Anvie/Types.h>

/* local includes */
#include "Device.h"

/* vulkan include */
#include <vulkan/vulkan.h>

/* NOTE:
 * From above it might look like it, but almost all functions are using the global
 * Vulkan state. It's using the `VkDevice` handle and `VkInstance` handles from it.
 * */

/* some forward declarations */
typedef struct XwWindow     XwWindow;
typedef struct RenderTarget RenderTarget;

/**
 * @b Swapchain images are treated as color attachments to @c RenderTarget objects.
 *
 * This struct tries to imitate @c DeviceImage as much as possible for consistency of code.
 * Some things that we don't really require in these images, we don't store. One such
 * example is @c VkDeviceMemory handle, because swapchain is the allocator of these images,
 * and not the plugin code. Another example is image dimensions as neither @c RenderTarget,
 * nor image view require image dimensions.
 *
 * @sa RenderTarget
 * @sa DeviceImage
 * */
typedef struct SwapchainImage {
    VkImage     image; /**< @b Swapchain image handle retrieved from swapchain. */
    VkImageView view;  /**< @b Image view created for corresponding image. */

    /**
     * @b This is same in all images of a swapchain.
     * The same value is also present in @c image_format variable inside parent swapchain.
     * This might seem redundant but it makes creation of @c RenderTarget less verbose.
     * */
    VkFormat format;
} SwapchainImage;

/**
 * @b Wrapper over VkSwapchainKHR handle and closely related objects.
 * */
typedef struct Swapchain {
    VkSwapchainKHR  swapchain;    /**< @b Swapchain created for the window. */
    VkExtent2D      image_extent; /**< @b Current swapchain image extent */
    VkFormat        image_format; /**< @b Format of image stored during swapchain creation. */
    Uint32          image_count;  /**< @b Number of images in swapchain. */
    SwapchainImage *images;       /**< @b Handle to images inside swapchain. */

    /** 
     * @b Total number of render targets is same as number of swapchain images.
     * */
    RenderTarget *render_targets;
    DeviceImage   depth_image; /**< @b Common depth image attachments to all render targets */

    /**
     * @b Where all command buffers will be allocated from for each render target
     *    command recording. 
     * */
    VkCommandPool cmd_pool;
} Swapchain;

Swapchain *swapchain_init (Swapchain *swapchain, VkSurfaceKHR surface, XwWindow *win);
Swapchain *swapchain_deinit (Swapchain *swapchain);
Swapchain *swapchain_reinit (Swapchain *swapchain, VkSurfaceKHR surface, XwWindow *win);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SWAPCHAIN_H
