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
#include "Swapchain.h"
#include "Vulkan.h"

/**************************************************************************************************/
/********************************** PRIVATE METHODS DECLARATIONS **********************************/
/**************************************************************************************************/

static inline RenderPass *
    default_render_pass_create_framebuffers (RenderPass *render_pass, Swapchain *swapchain);
static inline RenderPass *render_pass_destroy_framebuffers (RenderPass *render_pass);
static inline RenderPass *render_pass_create_frame_data (RenderPass *render_pass);
static inline RenderPass *render_pass_destroy_frame_data (RenderPass *render_pass);

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
        !default_render_pass_create_framebuffers (render_pass, swapchain) ||
            !render_pass_create_frame_data (render_pass),
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
        "Failed to create default graphics pipeline for default renderpass.\n"
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

    render_pass_destroy_framebuffers (render_pass);
    render_pass_destroy_frame_data (render_pass);

    vkDestroyRenderPass (device, render_pass->render_pass, Null);

    memset (render_pass, 0, sizeof (RenderPass));

    return render_pass;
}

/**************************************************************************************************/
/**************************************** PRIVATE METHODS *****************************************/
/**************************************************************************************************/

/**
 * @b Create framebuffers. 
 *
 * @param render_pass @c RenderPass object to create render targets for 
 * @param swapchain 
 *
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
static inline RenderPass *
    default_render_pass_create_framebuffers (RenderPass *render_pass, Swapchain *swapchain) {
    RETURN_VALUE_IF (!render_pass || !swapchain, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* create space for render target */
    render_pass->framebuffer_count = swapchain->image_count;
    render_pass->framebuffers      = ALLOCATE (VkFramebuffer, render_pass->framebuffer_count);
    RETURN_VALUE_IF (!render_pass->framebuffers, Null, ERR_OUT_OF_MEMORY);

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

    /* create command pool and buffer */
    for (Size s = 0; s < render_pass->framebuffer_count; s++) {
        /* create framebuffer for each render target */
        framebuffer_create_info.pAttachments = (VkImageView[]) {
            swapchain->images[s].view,  /* color attachment */
            swapchain->depth_image.view /* depth-stencil attachment */
        };

        VkResult res = vkCreateFramebuffer (
            device,
            &framebuffer_create_info,
            Null,
            render_pass->framebuffers + s
        );

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to create framebuffer for a render target. RET = %d\n",
            res
        );
    }

    return render_pass;
}

/**
 * @b Destroy @c RenderPass @c RenderTarget objects.
 * */
static inline RenderPass *render_pass_destroy_framebuffers (RenderPass *render_pass) {
    RETURN_VALUE_IF (!render_pass, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    /* destroy render target member objects */
    if (render_pass->framebuffers) {
        for (Size s = 0; s < render_pass->framebuffer_count; s++) {
            if (render_pass->framebuffers[s]) {
                vkDestroyFramebuffer (device, render_pass->framebuffers[s], Null);
                render_pass->framebuffers[s] = VK_NULL_HANDLE;
            }
        }

        FREE (render_pass->framebuffers);
    }

    return render_pass;
}

/**
 * @b Create @c FrameData objects for command recording and synchronization.
 *
 * @param render_pass @c RenderPass object to create render targets for 
 *
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
static inline RenderPass *render_pass_create_frame_data (RenderPass *render_pass) {
    RETURN_VALUE_IF (!render_pass, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* iterator */
    FrameData *frame_data = render_pass->frame_data;

    /* create command pool and buffer */
    for (Size s = 0; s < FRAME_LIMIT; s++) {
        /* create command pool */
        {
            /* directly reset command pool, instead of resetting buffers separately */
            VkCommandPoolCreateInfo command_pool_create_info = {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext            = Null,
                .flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                .queueFamilyIndex = vk.device.graphics_queue.family_index
            };

            VkResult res = vkCreateCommandPool (
                device,
                &command_pool_create_info,
                Null,
                &frame_data->command.pool
            );

            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to create Command Pool. RET = %d\n",
                res
            );
        }

        /* allocate command buffer for frame */
        {
            VkCommandBufferAllocateInfo command_buffer_allocate_info = {
                .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext              = Null,
                .commandPool        = frame_data->command.pool,
                .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };

            VkResult res = vkAllocateCommandBuffers (
                device,
                &command_buffer_allocate_info,
                &frame_data->command.buffer
            );

            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to allocate command buffers for a render target. RET = %d\n",
                res
            );
        }

        /* init sync structures */
        {
            VkFenceCreateInfo fence_create_info = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = 0,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT
            };

            VkResult res =
                vkCreateFence (device, &fence_create_info, Null, &frame_data->sync.render_fence);
            RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create Fence. RET = %d\n", res);

            VkSemaphoreCreateInfo semaphore_create_info =
                {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = Null, .flags = 0};

            res = vkCreateSemaphore (
                device,
                &semaphore_create_info,
                Null,
                &frame_data->sync.render_semaphore
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to create Semaphore. RET = %d\n",
                res
            );

            res = vkCreateSemaphore (
                device,
                &semaphore_create_info,
                Null,
                &frame_data->sync.present_semaphore
            );
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                Null,
                "Failed to create Semaphore. RET = %d\n",
                res
            );
        }

        /* move on to next frame_data */
        frame_data++;
    }

    return render_pass;
}

/**
 * @b Destroy @c RenderPass @c RenderTarget objects.
 *
 * @param render_pass 
 * 
 * @return @c render_pass on success.
 * @return @c Null otherwise.
 * */
static inline RenderPass *render_pass_destroy_frame_data (RenderPass *render_pass) {
    RETURN_VALUE_IF (!render_pass, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    FrameData *frame_data = render_pass->frame_data;

    /* destroy frame data member objects i*/
    for (Size s = 0; s < FRAME_LIMIT; s++) {
        if (frame_data->command.pool) {
            vkDestroyCommandPool (device, frame_data->command.pool, Null);
        }

        if (frame_data->sync.render_fence) {
            vkDestroyFence (device, frame_data->sync.render_fence, Null);
        }

        if (frame_data->sync.render_semaphore) {
            vkDestroySemaphore (device, frame_data->sync.render_semaphore, Null);
        }

        if (frame_data->sync.present_semaphore) {
            vkDestroySemaphore (device, frame_data->sync.present_semaphore, Null);
        }

        frame_data++;
    }

    memset (render_pass->frame_data, 0, sizeof (FrameData) * FRAME_LIMIT);

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
        !render_pass_destroy_framebuffers (render_pass),
        False,
        "Failed to destroy render pass framebuffers\n"
    );

    RETURN_VALUE_IF (
        !render_pass_destroy_frame_data (render_pass),
        False,
        "Failed to destroy render pass frame data\n"
    );

    RETURN_VALUE_IF (
        !default_render_pass_create_framebuffers (render_pass, swapchain),
        False,
        "Failed to create default render pass framebuffers\n"
    );

    RETURN_VALUE_IF (
        !render_pass_create_frame_data (render_pass),
        False,
        "Failed to create render pass frame data\n"
    );

    return True;
}
