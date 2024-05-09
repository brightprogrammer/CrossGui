/**
 * @file RenderPass.h
 * @date Sun, 28th April 2024
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

#ifndef ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDER_PASS_H
#define ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDER_PASS_H

#include <Anvie/Types.h>

/* vulkan includes */
#include <vulkan/vulkan.h>

/* local includes */
#include "GraphicsPipeline.h"

/* fwd declarations */
typedef struct Swapchain     Swapchain;
typedef struct RenderSubPass RenderSubPass;

typedef enum RenderPassType {
    RENDER_PASS_TYPE_UKNOWN = 0,
    RENDER_PASS_TYPE_DEFAULT,
    RENDER_PASS_TYPE_MAX
} RenderPassType;

/**
 * @b Frame command recording and synchronization data.
 * */
typedef struct FrameData {
    struct {
        /** @b Signaled to the CPU when corresponding render target is no longer being rendered to by 
         *     the GPU.*/
        VkFence render_fence;

        /** @b Signaled to the GPU when corresponding render target is no longer being rendered to by
         *     the GPU.*/
        VkSemaphore render_semaphore;

        /** @b Signaled to the GPU when corresponding render target is no longer being used for
         *     presentation to corresponding surface by the GPU. */
        VkSemaphore present_semaphore;
    } sync;

    /**
     * @b Command objects.
     *
     * There's a one-to-one correspondance between the command pool and command buffer.
     * */
    struct {
        /**
         * @b Command pool created with `VK_COMMAND_POOL_TRANSIENT_BIT` so that the command
         *    buffer can directly be reset, instead of resetting each buffer separately.
         * */
        VkCommandPool pool;

        /**
         * @b Buffer allocated from above command pool.
         * */
        VkCommandBuffer buffer;
    } command;
} FrameData;

#define FRAME_LIMIT 2

/**
 * @b RenderPass objects are pre-baked for each swapchain.
 * Meaning for each swapchain, there exists a set number of RenderPass objects,
 * and they are created the moment a new @c XuiGraphicsContext object is created.
 *
 * Now, even though a @c RenderPass owns the @c RenderTarget objects, there's this requirement,
 * where if the @c Swapchain which was used to create this @c RenderPass is ever recreated,
 * then we need to recreate the owned @c RenderTarget objects as well.
 *
 * To address this issue, @c RenderPass when being created using a given @c Swapchain object,
 * will register itself as reinit event handler to the given @c Swapchain. This allows
 * the @c RenderPass to own @c RenderTarget objects while automatically recreating them when
 * required.
 *
 * Note here that even though all @c RenderTarget objects will be recreated, we don't need to
 * recreate the @c VkRenderPass object too, because it does not have any direct attachment
 * with these @c RenderTarget objects. The converse is true, that @c RenderTarget is directly
 * attached with it's corresponding @c RenderPass.
 * */
typedef struct RenderPass {
    VkRenderPass render_pass;

    /**
     * @b Number of @c RenderTarget objects.
     * This value exactly matches with the total number of swapchain images in the
     * @c Swapchain object that was used to create this @c RenderPass.
     * */
    Size           framebuffer_count;
    VkFramebuffer *framebuffers; /**< @b RenderTarget objects in this @c RenderPass*/

    Uint8     frame_index;       /**< @b Index of current frame in use. */
    FrameData frame_data[FRAME_LIMIT];

    RenderPassType type;

    /**
     * @b Tagged union to store graphics pipelines for different renderpass type.
     * */
    union {
        GraphicsPipeline default_graphics;
    } pipelines;
} RenderPass;

RenderPass *render_pass_init_default (RenderPass *rp, Swapchain *swapchain);
RenderPass *render_pass_deinit (RenderPass *rp);
Bool        render_pass_wait_frame (RenderPass *rp);
Bool        render_pass_reset_frame (RenderPass *rp);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDER_PASS_H
