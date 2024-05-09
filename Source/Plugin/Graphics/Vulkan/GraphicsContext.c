/**
 * @file GraphicsContext.c
 * @date Thu, 25th April 2024
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

/* crossgui */
#include <Anvie/CrossGui/Graphics.h>

/* local includes */
#include "GraphicsContext.h"
#include "RenderPass.h"
#include "Vulkan.h"

#include <Anvie/CrossWindow/Vulkan.h>
#include <vulkan/vulkan_core.h>


/**
 * @b Create graphics context for Vulkan plugin.
 *
 * @param xwin @c Window to create graphics context for.
 * 
 * @return @c XuiGraphicsContext pointer on success.
 * @return @c Null otherwise.
 * */
XuiGraphicsContext *graphics_context_create (XwWindow *xwin) {
    RETURN_VALUE_IF (!xwin, Null, ERR_INVALID_ARGUMENTS);

    XuiGraphicsContext *gctx = NEW (XuiGraphicsContext);
    RETURN_VALUE_IF (!gctx, Null, ERR_OUT_OF_MEMORY);

    /* create swapchain */
    GOTO_HANDLER_IF (
        !swapchain_init (&gctx->swapchain, xwin),
        GCTX_FAILED,
        "Failed to create swapchain\n"
    );

    /* create default renderpass */
    GOTO_HANDLER_IF (
        !render_pass_init_default (&gctx->default_render_pass, &gctx->swapchain),
        GCTX_FAILED,
        "Failed to create default renderpass for new graphics context\n"
    );

    /* create uniform buffer to send GPU data */
    {
        GOTO_HANDLER_IF (
            !device_buffer_init (
                &gctx->ui_data,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                sizeof (Rect2D),
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                vk.device.graphics_queue.family_index
            ) || !gctx->ui_data.size,
            GCTX_FAILED,
            "Failed to create ui data uniform buffer for graphics context\n"
        );

        GOTO_HANDLER_IF (
            !graphics_pipeline_write_to_descriptor_set (
                &gctx->default_render_pass.pipelines.default_graphics,
                &gctx->ui_data
            ),
            GCTX_FAILED,
            "Failed to write UBO info to descriptor set\n"
        );
    }

    return gctx;

GCTX_FAILED:
    graphics_context_destroy (gctx);
    return Null;
}

/**
 * @b Destroy the given @x XuiGraphicsContext object.
 *
 * @param gctx @c XuiGraphicsContext object to be destroyed. 
 * */
void graphics_context_destroy (XuiGraphicsContext *gctx) {
    RETURN_IF (!gctx, ERR_INVALID_ARGUMENTS);

    if (gctx->ui_data.buffer) {
        device_buffer_deinit (&gctx->ui_data);
    }

    if (gctx->default_render_pass.render_pass) {
        render_pass_deinit (&gctx->default_render_pass);
    }

    if (gctx->swapchain.swapchain) {
        swapchain_deinit (&gctx->swapchain);
    }

    FREE (gctx);
}

/**
 * @b Resize the graphics context if window is resized.
 *
 * User must necessarily issue a call to this method, if window is resized.
 * 
 * @param gctx Graphics context to be resized.
 * @param xwin Window associated with this graphics context and was resized.
 *
 * @return True on success.
 * @return False otherwise.
 * */
Bool graphics_context_resize (XuiGraphicsContext *gctx, XwWindow *xwin) {
    RETURN_VALUE_IF (!gctx || !xwin, False, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        !swapchain_reinit (&gctx->swapchain, xwin),
        False,
        "Failed to resize graphics context.\n"
    );

    return True;
}
