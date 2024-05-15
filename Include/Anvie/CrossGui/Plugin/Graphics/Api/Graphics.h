/**
 * @file Graphics.h
 * @date Wed, 15th May 2024
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_GRAPHICS_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_GRAPHICS_H

/**
 * @b Plugin must render given 2D shape.
 *
 * This is a per-vertex draw call.  
 *
 * @param graphics_context The @c XuiGraphicsContext object created for @x xwin.
 * @param xwin @c XwWindow object used to create @c graphics context.
 * @param vertices Array of 2D vertices.
 * @param vertex_count Number of vertices.
 *
 * @return True if draw was successful.
 * @return False otherwise.
 * */
typedef XuiRenderStatus (*XuiGraphicsDrawRect2D) (
    XuiGraphicsContext *graphics_context,
    XwWindow           *xwin,
    Rect2D              rect
);

/**
 * @b Clear images of swapchain in given @x XuiGraphicsContext object.
 *
 * @param graphics_context 
 * @param xwin 
 *
 * @return @c XUI_RENDER_STATUS_OK on success.
 * @return @c XUI_RENDER_STATUS_ERR otherwise.
 * */
typedef XuiRenderStatus (*XuiGraphicsClear) (XuiGraphicsContext *graphics_context, XwWindow *xwin);

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_GRAPHICS_H
