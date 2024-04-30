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
#include "Vulkan.h"

Bool draw_rect_2d (XuiGraphicsContext *gctx, XwWindow *win, Rect2D rect, Color color) {
    RETURN_VALUE_IF (!gctx || !win, False, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

                          /* TODO: create command pool for each RenderTarget 
                           * update this code */

    RenderPass   *rp             = &gctx->default_render_pass;
    RenderTarget *render_targets = rp->render_targets;
    VkSurfaceKHR  surface        = gctx->surface;
    Swapchain    *swapchain      = &gctx->swapchain;

    /* reset command buffer and record draw commands again */
    res = vkResetCommandBuffer (surface->cmd_buffer, 0);
    GOTO_HANDLER_IF (
        res != VK_SUCCESS,
        DRAW_ERROR,
        "Failed to reset command buffer for recording new commands. RET = %d\n",
        res
    );

    VkCommandBuffer          cmd            = surface->cmd_buffer;
    VkCommandBufferBeginInfo cmd_begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = Null,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = Null
    };

    res = vkBeginCommandBuffer (cmd, &cmd_begin_info);
    GOTO_HANDLER_IF (
        res != VK_SUCCESS,
        DRAW_ERROR,
        "Failed to begin command buffer recording. RET = %d\n",
        res
    );

    VkClearValue clear_value = {.color = {{0, 0, 0, 1}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = Null,
        .renderPass  = surface->render_pass,
        .renderArea  = {.offset = {.x = 0, .y = 0}, .extent = swapchain->swapchain_image_extent},
        .framebuffer = surface->framebuffers[next_image_index],
        .clearValueCount = 1,
        .pClearValues    = &clear_value
    };

    vkCmdBeginRenderPass (cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass (cmd);

    res = vkEndCommandBuffer (cmd);
    GOTO_HANDLER_IF (
        res != VK_SUCCESS,
        DRAW_ERROR,
        "Failed to end command buffer recording. RET = %d\n",
        res
    );

    return True;
}
