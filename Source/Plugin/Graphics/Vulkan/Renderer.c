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
#include "Vulkan.h"

Renderer *renderer_init (Renderer *renderer) {
    RETURN_VALUE_IF (!renderer, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* create renderpass */
    {
        PRINT_ERR ("Given depth image is not in use yet! Fix this...\n");

        VkAttachmentDescription color_attachment = {
            .flags          = 0,
            .format         = color_image->format,
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
            vkCreateRenderPass (device, &render_pass_create_info, Null, &rt->render_pass);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to create Vulkan Render Pass. RET = %d\n",
            res
        );
    }

    return renderer;
}

Renderer *renderer_deinit (Renderer *renderer);
Bool      renderer_draw_rect_2d (Renderer *renderer, XuiGraphicsContext *gctx, Rect2D rect);
