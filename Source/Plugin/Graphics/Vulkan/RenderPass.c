/**
 * @file RenderPass.c
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

#include <Anvie/Common.h>
#include <vulkan/vulkan_core.h>

/* libc includes */
#include <memory.h>

/* local includes */
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Swapchain.h"
#include "Vulkan.h"

/**************************************************************************************************/
/********************************** PRIVATE METHODS DECLARATIONS **********************************/
/**************************************************************************************************/

static inline RenderPass *
    default_render_pass_create_render_targets (RenderPass *render_pass, Swapchain *swapchain);

static inline RenderPass *render_pass_destroy_render_targets (RenderPass *render_pass);

static inline Bool default_render_pass_swapchain_reinit_event_handler (
    RenderPass *render_pass,
    Swapchain  *swapchain
);

/**************************************************************************************************/
/***************************************** PUBLIC METHODS *****************************************/
/**************************************************************************************************/

/**
 * @b Initialize default render pass.
 *
 * @param render_pass
 * @param swapchain @c Swapchain object to create this render pass for.
 *
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
RenderPass *render_pass_init_default (RenderPass *render_pass, Swapchain *swapchain) {
    RETURN_VALUE_IF (!render_pass || !swapchain, Null, ERR_INVALID_ARGUMENTS);

    /* this is the default pass */
    render_pass->type = RENDER_PASS_TYPE_DEFAULT;

    VkDevice device = vk.device.logical;

    /* create render pass */
    {
        PRINT_ERR ("Given depth image is not in use yet! Fix this...\n");

        VkAttachmentDescription color_attachment = {
            .flags          = 0,
            .format         = swapchain->image_format,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        VkAttachmentReference color_attachment_reference = {
            .attachment = 0, /* index of color attachment in render pass */
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkAttachmentDescription depth_attachment = {
            .flags          = 0,
            .format         = swapchain->depth_image.format,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };
        VkAttachmentReference depth_attachment_reference = {
            .attachment = 1, /* index of depth attachment in render pass */
            .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };


        VkSubpassDescription subpass = {
            .flags                   = 0,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount    = 0,
            .pInputAttachments       = Null,
            .colorAttachmentCount    = 1,
            .pColorAttachments       = &color_attachment_reference,
            .pResolveAttachments     = Null,
            .pDepthStencilAttachment = &depth_attachment_reference,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments    = Null
        };

        VkAttachmentDescription render_pass_attachments[] = {color_attachment, depth_attachment};
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
            vkCreateRenderPass (device, &render_pass_create_info, Null, &render_pass->render_pass);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to create Vulkan Render Pass. RET = %d\n",
            res
        );
    }

    /* create render targets */
    GOTO_HANDLER_IF (
        !default_render_pass_create_render_targets (render_pass, swapchain),
        INIT_FAILED,
        "Failed to create render targets for render pass\n"
    );

    /* create graphics pipeline for subpass 0 */
    GOTO_HANDLER_IF (
        !graphics_pipeline_init_default (
            &render_pass->pipelines.default_graphics.pipeline,
            render_pass,
            swapchain
        ),
        INIT_FAILED,
        "Failed to create default graphics pipeline for default renderpass."
    );

    /* finally register this renderpass to handle swapchain reinit events */
    GOTO_HANDLER_IF (
        !swapchain_register_reinit_handler (
            swapchain,
            default_render_pass_swapchain_reinit_event_handler,
            render_pass
        ),
        INIT_FAILED,
        "Failed to register render pass to swapchain for reinit event handling\n"
    );

    return render_pass;

INIT_FAILED:
    render_pass_deinit (render_pass);
    return Null;
}


/**
 * @b De-initialize given render pass.
 *
 * @param render_pass
 *
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
RenderPass *render_pass_deinit (RenderPass *render_pass) {
    RETURN_VALUE_IF (!render_pass, Null, ERR_INVALID_ARGUMENTS);

    if (render_pass->type == RENDER_PASS_TYPE_DEFAULT) {
        graphics_pipeline_deinit (&render_pass->pipelines.default_graphics.pipeline);
    }

    VkDevice device = vk.device.logical;

    render_pass_destroy_render_targets (render_pass);

    vkDestroyRenderPass (device, render_pass->render_pass, Null);

    memset (render_pass, 0, sizeof (RenderPass));

    return render_pass;
}

/**************************************************************************************************/
/**************************************** PRIVATE METHODS *****************************************/
/**************************************************************************************************/

/**
 * @b Create render targets for this renderpass, and store them back into the @c RenderPass object.
 *
 * @param render_pass @c RenderPass object to create render targets for 
 * @param swapchain 
 *
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
static inline RenderPass *
    default_render_pass_create_render_targets (RenderPass *render_pass, Swapchain *swapchain) {
    RETURN_VALUE_IF (!render_pass || !swapchain, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* create space for render target */
    render_pass->render_target_count = swapchain->image_count;
    render_pass->render_targets =
        REALLOCATE (render_pass->render_targets, RenderTarget, render_pass->render_target_count);
    RETURN_VALUE_IF (!render_pass->render_targets, Null, ERR_OUT_OF_MEMORY);

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = Null,
        .flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = vk.device.graphics_queue.family_index
    };

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = Null,
        .commandPool        = VK_NULL_HANDLE,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkFramebufferCreateInfo framebuffer_create_info = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = Null,
        .flags           = 0,
        .renderPass      = render_pass->render_pass,
        .attachmentCount = 2,
        .pAttachments    = Null,
        .width           = swapchain->image_extent.width,
        .height          = swapchain->image_extent.height,
        .layers          = 1
    };

    /* iterator */
    RenderTarget *render_target = render_pass->render_targets;

    /* create command pool and buffer */
    for (Size s = 0; s < render_pass->render_target_count; s++) {
        VkResult res = vkCreateCommandPool (
            device,
            &command_pool_create_info,
            Null,
            &render_target->command.pool
        );
        RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Command Pool. RET = %d\n", res);

        /* allocate command buffer for frame */
        command_buffer_allocate_info.commandPool = render_target->command.pool;
        VkCommandBuffer cmdbuf                   = VK_NULL_HANDLE;
        res = vkAllocateCommandBuffers (device, &command_buffer_allocate_info, &cmdbuf);
        render_target->command.buffer = cmdbuf;
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to allocate command buffers for a render target. RET = %d\n",
            res
        );

        /* create framebuffer for each render target */
        framebuffer_create_info.pAttachments = (VkImageView[]) {
            swapchain->images[s].view,  /* color attachment */
            swapchain->depth_image.view /* depth-stencil attachment */
        };
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        res = vkCreateFramebuffer (device, &framebuffer_create_info, Null, &framebuffer);
        render_pass->render_targets[s].framebuffer = framebuffer;
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to create framebuffer for a render target. RET = %d\n",
            res
        );

        /* move on to next render target */
        render_target++;
    }

    return render_pass;
}

/**
 * @b Destroy @c RenderPass @c RenderTarget objects.
 * */
static inline RenderPass *render_pass_destroy_render_targets (RenderPass *render_pass) {
    RETURN_VALUE_IF (!render_pass, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* destroy render target member objects */
    if (render_pass->render_targets) {
        for (Size s = 0; s < render_pass->render_target_count; s++) {
            RenderTarget *rt = render_pass->render_targets + s;

            if (rt->framebuffer) {
                vkDestroyFramebuffer (device, rt->framebuffer, Null);
            }

            if (rt->command.pool) {
                vkDestroyCommandPool (device, rt->command.pool, Null);
            }
        }

        /* invalidate contents */
        memset (
            render_pass->render_targets,
            0,
            sizeof (RenderTarget) * render_pass->render_target_count
        );

        FREE (render_pass->render_targets);
    }

    return render_pass;
}

/**
 * @b Event handler for default @c RenderPass to handle @c Swapchain reinit events.
 *
 * @param render_pass @c RenderPass object to recreate it's @c RenderTarget objects.
 * @param swapchain @c Swapchain object used to create corresponding @c RenderPass.
 * */
static inline Bool default_render_pass_swapchain_reinit_event_handler (
    RenderPass *render_pass,
    Swapchain  *swapchain
) {
    RETURN_VALUE_IF (!render_pass || !swapchain, False, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        !render_pass_destroy_render_targets (render_pass) ||
            !default_render_pass_create_render_targets (render_pass, swapchain),
        False,
        "Failed to recreate swapchain render targets\n"
    );

    return True;
}
