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

#include <Anvie/Common.h>

/* crossgui-graphics-api */
#include <Anvie/CrossGui/Plugin/Graphics/Api/Mesh2D.h>

/* local includes */
#include "Anvie/CrossGui/Plugin/Graphics/Api/Common.h"
#include "Device.h"
#include "GraphicsContext.h"
#include "MeshManager.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "Swapchain.h"
#include "Vulkan.h"

#include <vulkan/vulkan_core.h>

typedef struct BeginEndInfo {
    FrameData    *frame_data;
    VkFramebuffer framebuffer;
    Uint32        image_index;
} BeginEndInfo;

/**************************************************************************************************/
/********************************** PRIVATE METHOD DECLARATIONS ***********************************/
/**************************************************************************************************/

static XuiRenderStatus begin_frame (
    RenderPass   *render_pass,
    Swapchain    *swapchain,
    XwWindow     *win,
    BeginEndInfo *begin_info
);
static XuiRenderStatus end_frame (
    RenderPass   *render_pass,
    Swapchain    *swapchain,
    XwWindow     *win,
    BeginEndInfo *end_info
);

/**************************************************************************************************/
/*********************************** PUBLIC METHOD DEFINITIONS ************************************/
/**************************************************************************************************/

XuiRenderStatus
    gfx_draw_2d (XuiGraphicsContext *gctx, XwWindow *win, XuiMeshInstance2D *mesh_instance) {
    RETURN_VALUE_IF (!gctx || !win || !mesh_instance, XUI_RENDER_STATUS_ERR, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        !mesh_manager_add_mesh_instance_2d (&vk.mesh_manager, mesh_instance),
        XUI_RENDER_STATUS_ERR,
        "Failed to add mesh instance for drawing"
    );

    return XUI_RENDER_STATUS_OK;
}

XuiRenderStatus gfx_display (XuiGraphicsContext *gctx, XwWindow *win) {
    RETURN_VALUE_IF (!gctx, XUI_RENDER_STATUS_ERR, ERR_INVALID_ARGUMENTS);

    RenderPass       *render_pass      = &gctx->default_render_pass;
    Swapchain        *swapchain        = &gctx->swapchain;
    GraphicsPipeline *default_pipeline = &render_pass->pipelines.default_graphics;

    BeginEndInfo info = {0};

    /* begin frame rendering and command recording,
     * will get new frame data in info struct */
    XuiRenderStatus status = begin_frame (render_pass, swapchain, win, &info);
    if (status != XUI_RENDER_STATUS_OK) {
        return status;
    }

    VkCommandBuffer cmd = info.frame_data->command.buffer;

    /* Since we're not clearing color images, the transition won't happen automatically.
     * For this, we need to transition these images ourselves before we begin renderpass */
    if (swapchain->is_reinited) {
        PRINT_ERR ("Changed image layout on renit\n");

        /* wait for pending device operations to complete */
        vkDeviceWaitIdle (vk.device.logical);

        /* change layout of all images */
        swapchain_change_image_layout (
            swapchain,
            info.image_index, /* swapchain image index*/
            cmd,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        );

        device_image_change_layout (
            &swapchain->depth_image,
            cmd,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        );

        /* mark this image as cleared */
        swapchain->clear_mask  &= ~(1 << info.image_index);
        swapchain->is_reinited  = !!swapchain->clear_mask;
    } else {
        swapchain_change_image_layout (
            swapchain,
            info.image_index,
            cmd,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        );
    }

    /* begin render pass */
    {
        VkRenderPassBeginInfo render_pass_begin_info = {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = Null,
            .renderPass      = render_pass->render_pass,
            .renderArea      = {.offset = {.x = 0, .y = 0}, .extent = swapchain->image_extent},
            .framebuffer     = info.framebuffer,
            .clearValueCount = 0
        };

        vkCmdBeginRenderPass (cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    vkCmdBindPipeline (cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, default_pipeline->pipeline);


    /* issue a draw call for each batch just once */
    for (Size s = 0; s < vk.mesh_manager.batches_2d.count; s++) {
        /* get batch */
        MeshInstanceBatch2D *batch = vk.mesh_manager.batches_2d.data + s;

        /* skip if batch as no instances */
        if (!batch->instances.count) {
            continue;
        }

        /* resize if required */
        Size batch_size_in_bytes = sizeof (XuiMeshInstance2D) * batch->instances.count;
        if (gctx->batch_data.size < batch_size_in_bytes) {
            RETURN_VALUE_IF (
                !device_buffer_resize (&gctx->batch_data, batch_size_in_bytes),
                XUI_RENDER_STATUS_ERR,
                "Failed to resize UBO to send batch data\n"
            );
        }

        /* send batch data as vertex data but per instance */
        device_buffer_memcpy (&gctx->batch_data, batch->instances.data, batch_size_in_bytes);

        /* get mesh data */
        MeshData2D *mesh =
            mesh_manager_get_mesh_data_by_type_2d (&vk.mesh_manager, batch->mesh_type);

        PRINT_ERR ("Mesh has indices count = %zu\n", mesh->vertex_count);

        /* bind shape data */
        vkCmdBindVertexBuffers (
            cmd,
            0,                                                           /* first binding */
            2,                                                           /* binding count */
            (VkBuffer[]) {mesh->vertex.buffer, gctx->batch_data.buffer}, /* buffers */
            (VkDeviceSize[]) {0, 0}                                      /* offsets */
        );

        vkCmdBindIndexBuffer (cmd, mesh->index.buffer, 0, VK_INDEX_TYPE_UINT32);

        /* draw */
        vkCmdDrawIndexed (cmd, mesh->index_count, batch->instances.count, 0, 0, 0);
    }


    /* end render pass */
    vkCmdEndRenderPass (cmd);

    /* end command recording, submit for rendering and present to screen */
    return end_frame (render_pass, swapchain, win, &info);
}

/**
 * @b Clear next frame instead of drawing something to it.
 *
 * @param gctx
 * @param win
 *
 * @return @c XUI_RENDER_STATUS_OK on success.
 * @return @c XUI_RENDER_STATUS_ERR otherwise.
 * */
XuiRenderStatus gfx_clear (XuiGraphicsContext *gctx, XwWindow *win) {
    RETURN_VALUE_IF (!gctx || !win, XUI_RENDER_STATUS_ERR, ERR_INVALID_ARGUMENTS);

    RenderPass *render_pass = &gctx->default_render_pass;
    Swapchain  *swapchain   = &gctx->swapchain;
    FrameData  *frame_data  = render_pass->frame_data + (render_pass->frame_index % FRAME_LIMIT);

    /* wait for prending operations */
    {
        /* wait for rendering operations to complete for selected fence */
        VkResult res =
            vkWaitForFences (vk.device.logical, 1, &frame_data->sync.render_fence, True, 1e9);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Timeout (1s) while waiting for fences. RET = %d\n",
            res
        );

        /* need to reset fence before we use it again */
        res = vkResetFences (vk.device.logical, 1, &frame_data->sync.render_fence);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Failed to reset fences. RET = %d\n",
            res
        );
    }

    /* get command buffer handle */
    VkCommandBuffer cmd = frame_data->command.buffer;

    /* reset command pool and begin command recording */
    {
        /* reset command buffer and record draw commands again */
        {
            VkResult res = vkResetCommandPool (vk.device.logical, frame_data->command.pool, 0);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to reset command buffer for recording new commands. RET = %d\n",
                res
            );
        }

        {
            VkCommandBufferBeginInfo cmd_begin_info = {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext            = Null,
                .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = Null
            };

            VkResult res = vkBeginCommandBuffer (cmd, &cmd_begin_info);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to begin command buffer recording. RET = %d\n",
                res
            );
        }
    }

    /* clear all images in swapchain */
    for (Size s = 0; s < swapchain->image_count; s++) {
        swapchain_clear_image (
            swapchain,
            s,
            cmd,
            (VkClearColorValue) {
                .float32 = {0, 0, 0, 1}
        }
        );
    }

    device_image_clear (
        &swapchain->depth_image,
        cmd,
        (VkClearValue) {
            .depthStencil = {.depth = 1.f, .stencil = 0.f}
    }
    );
    /* end command recording and submit */
    {
        /* end recording */
        {
            VkResult res = vkEndCommandBuffer (cmd);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to end command buffer recording. RET = %d\n",
                res
            );
        }

        /* submit for rendering */
        {
            /* wait when rendered image is being presented */
            VkSubmitInfo submit_info = {
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext                = Null,
                .waitSemaphoreCount   = 0,
                .pWaitSemaphores      = Null,
                .pWaitDstStageMask    = Null,
                .signalSemaphoreCount = 0,
                .pSignalSemaphores    = Null,
                .commandBufferCount   = 1,
                .pCommandBuffers      = &cmd
            };

            VkResult res = vkQueueSubmit (
                vk.device.graphics_queue.handle,
                1,
                &submit_info,
                frame_data->sync.render_fence
            );

            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to submit command buffers for execution. RET = %d\n",
                res
            );
        }
    }
    return XUI_RENDER_STATUS_OK;
}


/**************************************************************************************************/
/*********************************** PRIVATE METHOD DEFINITIONS ***********************************/
/**************************************************************************************************/

/**
 * @b Begin frame rendering.
 *
 * - Get the next image available index
 * - Begin command buffer recording
 * - Get corresponding framebuffer
 * - Store all this in @c BeginEndInfo
 *
 * @param render_pass
 * @param swapchain
 * @param win
 * @param begin_info Where all the data will be stored.
 *
 * @return @c XUI_RENDER_STATUS_OK on success.
 * @return @c XUI_RENDER_STATUS_CONTINUE if a recoverable error occured.
 * @return @c XUI_RENDER_STATUS_ERR otherwise.
 * */
static XuiRenderStatus begin_frame (
    RenderPass   *render_pass,
    Swapchain    *swapchain,
    XwWindow     *win,
    BeginEndInfo *begin_info
) {
    RETURN_VALUE_IF (
        !render_pass || !win || !swapchain || !begin_info,
        XUI_RENDER_STATUS_ERR,
        ERR_INVALID_ARGUMENTS
    );

    VkDevice device = vk.device.logical;

    FrameData *frame_data  = render_pass->frame_data + (render_pass->frame_index++ % FRAME_LIMIT);
    begin_info->frame_data = frame_data;

    /* get next image index */
    Uint32 image_index = -1;
    {
        VkResult res = vkWaitForFences (device, 1, &frame_data->sync.render_fence, True, 1e9);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Timeout (1s) while waiting for fences. RET = %d\n",
            res
        );

        /* get next image index */
        {
            res = vkAcquireNextImageKHR (
                device,
                swapchain->swapchain,
                1e9, /* 1e9 ns = 1 s */
                frame_data->sync.present_semaphore,
                Null,
                &image_index
            );

            /* recoverable error cases */
            if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
                RETURN_VALUE_IF (
                    !swapchain_reinit (swapchain, win),
                    XUI_RENDER_STATUS_ERR,
                    "Failed to reinit swapchain\n"
                );

                return XUI_RENDER_STATUS_CONTINUE;
            }

            /* irrecoverable */
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to get next image index from swapchain. RET = %d\n",
                res
            );
        }

        /* need to reset fence before we use it again */
        res = vkResetFences (device, 1, &frame_data->sync.render_fence);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Failed to reset fences. RET = %d\n",
            res
        );
    }
    begin_info->image_index = image_index;
    begin_info->framebuffer = render_pass->framebuffers[image_index];

    VkCommandBuffer cmd = frame_data->command.buffer;

    /* reset command pool and begin command recording */
    {
        /* reset command buffer and record draw commands again */
        {
            VkResult res = vkResetCommandPool (vk.device.logical, frame_data->command.pool, 0);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to reset command buffer for recording new commands. RET = %d\n",
                res
            );
        }

        {
            VkCommandBufferBeginInfo cmd_begin_info = {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext            = Null,
                .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = Null
            };

            VkResult res = vkBeginCommandBuffer (cmd, &cmd_begin_info);
            RETURN_VALUE_IF (
                res != VK_SUCCESS,
                XUI_RENDER_STATUS_ERR,
                "Failed to begin command buffer recording. RET = %d\n",
                res
            );
        }
    }

    return XUI_RENDER_STATUS_OK;
}

/**
 * @b End frame rendering.
 *
 * Uses the data provided in @c end_info returned by
 * @c begin_frame method.
 *
 * @param render_pass
 * @param swapchain
 * @param win
 * @param end_info @c BeginEndInfo returned by @c begin_frame method 
 *
 * @return @c XUI_RENDER_STATUS_OK, XUI_RENDER_STATUS_CONTINUE on success.
 * @return @c XUI_RENDER_STATUS_ERR otherwise.
 * */
static XuiRenderStatus end_frame (
    RenderPass   *render_pass,
    Swapchain    *swapchain,
    XwWindow     *win,
    BeginEndInfo *end_info
) {
    RETURN_VALUE_IF (
        !render_pass || !win || !swapchain || !end_info,
        XUI_RENDER_STATUS_ERR,
        ERR_INVALID_ARGUMENTS
    );

    FrameData      *frame_data = end_info->frame_data;
    VkCommandBuffer cmd        = frame_data->command.buffer;

    VkResult res = vkEndCommandBuffer (cmd);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        XUI_RENDER_STATUS_ERR,
        "Failed to end command buffer recording. RET = %d\n",
        res
    );

    /* submit for rendering */
    {
        /* wait when rendered image is being presented */
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit_info = {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = Null,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &frame_data->sync.present_semaphore,
            .pWaitDstStageMask    = &wait_stage,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &frame_data->sync.render_semaphore,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd
        };

        VkResult res = vkQueueSubmit (
            vk.device.graphics_queue.handle,
            1,
            &submit_info,
            frame_data->sync.render_fence
        );

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Failed to submit command buffers for execution. RET = %d\n",
            res
        );
    }

    /* submit for presentation to surface */
    {
        VkPresentInfoKHR present_info = {
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = Null,
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain->swapchain,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &frame_data->sync.render_semaphore,
            .pImageIndices      = &end_info->image_index
        };

        VkResult res = vkQueuePresentKHR (vk.device.graphics_queue.handle, &present_info);

        if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
            RETURN_VALUE_IF (
                !swapchain_reinit (swapchain, win),
                XUI_RENDER_STATUS_ERR,
                "Failed to reinit swapchain\n"
            );

            return XUI_RENDER_STATUS_CONTINUE;
        }

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            XUI_RENDER_STATUS_ERR,
            "Failed to present rendered images to surface. RET = %d\n",
            res
        );
    }

    return XUI_RENDER_STATUS_OK;
}
