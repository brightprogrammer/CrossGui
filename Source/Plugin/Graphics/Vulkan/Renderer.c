/**
 * @file Renderer.c
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

/* local includes */
#include "Renderer.h"

#include "GraphicsContext.h"
#include "RenderTarget.h"
#include "Swapchain.h"
#include "Vulkan.h"

Bool draw_rect_2d (XuiGraphicsContext *gctx, XwWindow *win, Rect2D rect, Color color) {
    UNUSED (rect);
    RETURN_VALUE_IF (!gctx || !win, False, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    VkRenderPass  render_pass    = gctx->default_render_pass.render_pass;
    RenderTarget *render_targets = gctx->default_render_pass.render_targets;
    VkExtent2D    image_extent   = gctx->swapchain.image_extent;

    Uint32        image_index = swapchain_begin_frame (&gctx->swapchain, win);
    RenderTarget *rt          = render_targets + image_index;
    VkFramebuffer framebuffer = rt->framebuffer;

    /* reset command buffer and record draw commands again */
    VkResult res = vkResetCommandPool (device, rt->command.pool, 0);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to reset command buffer for recording new commands. RET = %d\n",
        res
    );

    /* begin command buffer recording */
    VkCommandBuffer cmd = rt->command.buffer;
    {
        VkCommandBufferBeginInfo cmd_begin_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = Null,
            .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = Null
        };

        res = vkBeginCommandBuffer (cmd, &cmd_begin_info);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            False,
            "Failed to begin command buffer recording. RET = %d\n",
            res
        );
    }

    /* begin render pass */
    {
        VkClearValue color_clear_value = {.color = {{color.r, color.g, color.b, color.a}}};
        VkClearValue depth_clear_value = {
            .depthStencil = {.depth = 0.f, .stencil = 0.f}
        };

        VkRenderPassBeginInfo render_pass_begin_info = {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = Null,
            .renderPass      = render_pass,
            .renderArea      = {.offset = {.x = 0, .y = 0}, .extent = image_extent},
            .framebuffer     = framebuffer,
            .clearValueCount = 2,
            .pClearValues    = (VkClearValue[]){color_clear_value, depth_clear_value}
        };

        vkCmdBeginRenderPass (cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    /* end render pass */
    vkCmdEndRenderPass (cmd);

    res = vkEndCommandBuffer (cmd);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        -1,
        "Failed to end command buffer recording. RET = %d\n",
        res
    );

    swapchain_end_frame (&gctx->swapchain, win, cmd, image_index);

    return True;
}
