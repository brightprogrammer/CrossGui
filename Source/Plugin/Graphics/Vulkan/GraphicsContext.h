/**
 * @file GraphicsContext.h
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

#ifndef ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H
#define ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H

/* local includes */
#include "RenderPass.h"
#include "Swapchain.h"

typedef struct XuiGraphicsContext {
    VkSurfaceKHR surface;
    Swapchain    swapchain;
    RenderPass   default_render_pass;

    /**
     * @b Device buffer storing mesh instance data to be passed to GPU
     * just before a draw call.
     * */
    DeviceBuffer batch_data;
} XuiGraphicsContext;

XuiGraphicsContext *graphics_context_create (XwWindow *xwin);
void                graphics_context_destroy (XuiGraphicsContext *gctx);
Bool                graphics_context_resize (XuiGraphicsContext *gctx, XwWindow *xwin);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H
