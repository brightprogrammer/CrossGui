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
 * [01-05-2024] : brightprogrammer 
 *      - Follow-up changelogs are updated in README.md in the same folder
 * [27-04-2024] : brightprogrammer
 *      - Renamed file from Surface.h to Swapchain.h
 *      - Removed everything from Surface object outside of it, and moved the VkSurfaceKHR handle 
 *        to XuiGraphicsContext object.
 *      - New objects created :
 *        - SwapchainSyncObjects
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
#include <vulkan/vulkan_core.h>

/* NOTE:
 * From above it might look like it, but almost all functions are using the global
 * Vulkan state. It's using the `VkDevice` handle and `VkInstance` handles from it.
 * */

/* some forward declarations */
typedef struct XwWindow     XwWindow;
typedef struct RenderTarget RenderTarget;
typedef struct RenderPass   RenderPass;
typedef struct Swapchain    Swapchain;

/**
 * @b Swapchain images are treated as color attachments to @c RenderTarget objects.
 *
 * This struct tries to imitate @c DeviceImage as much as possible for consistency of code.
 * Some things that we don't really require in these images, we don't store. One such
 * example is @c VkDeviceMemory handle, because swapchain is the allocator of these images,
 * and not the plugin code. Another example is Swapchain image size (extent, dimension).
 *
 * @sa RenderTarget
 * @sa DeviceImage
 * */
typedef struct SwapchainImage {
    VkImage     image; /**< @b Swapchain image handle retrieved from swapchain. */
    VkImageView view;  /**< @b Image view created for corresponding image. */
} SwapchainImage;

/**
 * @b A set of required render target syncrhonization primitves.
 *
 * Instead of each @c RenderTarget object having a unique set of sync objects associated
 * it, corresponding number of sync objects are owned by the related @c Swapchain object.
 * This design is strongly under the assumption that the GPU can run only a single render pass,
 * at any given moment. This assumption is under the assumption that a single swapchain 
 * can present only one single image at a time. This assumption is under the assumption
 * that the swapchain extension for host platform is written by sane developers.
 * */
typedef struct SwapchainSyncObjects {
} SwapchainSyncObjects;

/**
 * @c RenderTarget objects attached with @c RenderPass objects will go in invalid state if
 * after swapchain recreation, new @c RenderTarget objects are not created, by using new
 * data from swapchain.
 *
 * Each @c RenderPass has it's own way of re-creating it's @c RenderTarget objects.
 * This data will be used to store the handler, and the @c RenderPass object that wants to 
 * handle reinit events from @c Swapchain.
 *
 * This design might give rise to use-after-free bugs, but if I keep the @c RenderPass objects 
 * static and don't free them as long as the plugin is active, everything should work smoothly.
 * This is the plan anyway, I'm not creating a genric graphics renderer, I just need a set
 * rendering styles and pipelines, and that's it!
 * */
typedef Bool (*SwapchainReinitHandler) (RenderPass *render_pass, Swapchain *swapchain);
typedef struct SwapchainReinitHandlerData {
    SwapchainReinitHandler handler;
    RenderPass            *render_pass;
} SwapchainReinitHandlerData;

/**
 * @b Wrapper over VkSwapchainKHR handle and closely related objects.
 * */
typedef struct Swapchain {
    VkSurfaceKHR   surface;       /**< @b Surface associated with this swapchain. */
    VkSwapchainKHR swapchain;     /**< @b Swapchain created for the window. */

    VkExtent2D      image_extent; /**< @b Current swapchain image extent */
    VkFormat        image_format; /**< @b Format of image stored during swapchain creation. */
    Uint32          image_count;  /**< @b Number of images in swapchain. */
    SwapchainImage *images;       /**< @b Handle to images inside swapchain. */
    DeviceImage     depth_image;  /**< @b Common depth image attachments to all render targets */

    /**
     * @c This matches the total number of RenderPass objects using color attachments from 
     *    this swapchain.
     *
     * Upon swapchain recreation, the @c Swapchain code will go through each of these methods
     * to inform the registered @c RenderPass objects that they need to take proper action
     * for this event.
     * */
    SwapchainReinitHandlerData *reinit_handlers;
    Size                        reinit_handler_count;    /**< @b How many have we stored? */
    Size                        reinit_handler_capacity; /**< @b How many can we store? */
} Swapchain;

Swapchain *swapchain_init (Swapchain *swapchain, XwWindow *win);
Swapchain *swapchain_deinit (Swapchain *swapchain);
Swapchain *swapchain_reinit (Swapchain *swapchain, XwWindow *win);
Bool       swapchain_register_reinit_handler (
          Swapchain             *swapchain,
          SwapchainReinitHandler handler,
          RenderPass            *render_pass
      );

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SWAPCHAIN_H
