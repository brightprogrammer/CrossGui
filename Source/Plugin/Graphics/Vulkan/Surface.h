/**
 * @file Surface.h
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

#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H

#include <Anvie/Types.h>

/* vulkan include */
#include <vulkan/vulkan.h>

/* NOTE:
 * From above it might look like it, but almost all functions of surface is using the global
 * Vulkan state. It's using the `VkDevice` handle and `VkInstance` handles from it.
 * */

typedef struct Vulkan   Vulkan;
typedef struct XwWindow XwWindow;

typedef struct Surface {
    VkSurfaceKHR surface;                  /**< @b Surface created for corrsponding XwWindow. */

    VkSwapchainKHR swapchain;              /**< @b Swapchain created for the window. */
    VkExtent2D     swapchain_image_extent; /**< @b Current swapchain image extent */
    VkFormat swapchain_image_format;    /**< @b Format of image stored during swapchain creation. */
    Uint32   swapchain_image_count;     /**< @b Number of images in swapchain. */
    VkImage *swapchain_images;          /**< @b Handle to images inside swapchain. */
    VkImageView *swapchain_image_views; /**< @b Image views created for images in swapchain. */
} Surface;

Surface *surface_init (Surface *surf, XwWindow *win);
Surface *surface_deinit (Surface *surf);
Surface *surface_recreate_swapchain (Surface *surf, XwWindow *win);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H
