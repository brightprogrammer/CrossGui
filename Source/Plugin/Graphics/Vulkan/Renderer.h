/**
 * @file Renderer.h
 * @date Sat, 27th April 2024
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

#ifndef ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H
#define ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H

#include <Anvie/CrossGui/Graphics.h>

/* local includes */
#include "GraphicsContext.h"
#include "ShaderResourceBinding.h"

/**
 * @b A set of required render target syncrhonization primitves.
 *
 * To allow concurrency, we must assign each render target it's own set of syncrhonization
 * primitives.
 * */
typedef struct RenderTargetSyncObjects {
    /** @b Signaled to the CPU when corresponding render target is no longer being rendered to by 
     *     the GPU.*/
    VkFence render_fence;

    /** @b Signaled to the GPU when corresponding render target is no longer being rendered to by
     *     the GPU.*/
    VkSemaphore render_semaphore;

    /** @b Signaled to the GPU when corresponding render target is no longer being used for
     *     presentation to corresponding surface by the GPU. */
    VkSemaphore present_semaphore;
} RenderTargetSyncObjects;

/**
 * @b Contains data about a single render target.
 *
 * A render target is where the end render result is stored. The render target is then used
 * to display it's content to a surface. There's a many-to-one connection between the set of
 * render targets and the surface where they can be presented to.
 * */
typedef struct RenderTarget {
    /**
     * @b Each render target has it's own command buffer for recording.
     *    This allows concurrency to be possible. Meaning, we can record 
     *    rendering commands for multiple render targets at the same time.
     * */
    VkCommandBuffer cmd_buffer;

    /**
     * @b If we're aiming for concurrency, we need independent syncrhonization
     *    primitves as well.
     * */
    RenderTargetSyncObjects sync;

    /**
     * @b A framebuffer is the actual render target inside this abstracted away object.
     *    This is where the GPU will actually output the rendered results.
     * */
    VkFramebuffer framebuffer;

    /**
     * @b Color attachment is where the framebuffer's color data will be stored.
     *    There's one more attachment (the depth attachment), but that can be
     *    shared between multiple render targets.
     * */
    VkImageView color_attachment;
} RenderTarget;

/**
 * @b 2D Renderer.
 *
 * The idea of a renderer is to have a common renderer for all windows/surfaces.
 * CrossGui does not restrict user-code to have only one single window/surface, otherwise, it
 * just wouldn't be a GUI library anymore.
 *
 * Therefore we must make significant attempts to keep as many things common and shareable
 * between multiple windows.
 * */
typedef struct Renderer {
    /**
     * @b For now there's only one single renderpass, which only has one single subpass.
     * 
     * I don't care about shader effects for now. I just need something working, and render pass
     * abstraction/refactoring comes very late down the road acc. to me.
     * */
    VkRenderPass          render_pass;
    ShaderPipeline        pipeline;
    ShaderResourceBinding shader_resource_binding;
    DeviceImage           depth_image;
} Renderer;

Bool draw_rect_2d (XuiGraphicsContext *gctx, Rect2D scale);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H
