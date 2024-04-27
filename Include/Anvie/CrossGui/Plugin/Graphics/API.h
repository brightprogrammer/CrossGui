/**
 * @file API.h
 * @date Sat, 20th April 2024
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_H

#include <Anvie/Types.h>

/* crosswindow */
#include <Anvie/CrossWindow/Window.h>

/* crossgui */
#include <Anvie/CrossGui/Graphics.h>

/**
 * @b Opaque structure, defined by the plugin.
 *
 * Rendered image presentation method is different in different rendering APIs.
 * To abstract this away, I've decided to use a GraphicsContext, that's plugin
 * dependent. This needs to be passed to plugin whenever we need to render something.
 *
 * One can think of GraphicsContext having one-to-one correspondence with window.
 * */
typedef struct XuiGraphicsContext XuiGraphicsContext;

/**
 * @b Create a graphics context for given window.
 *
 * @param window 
 *
 * @return @c XuiGraphicsContext opaque object pointer on success.
 * @return @c Null otherwise.
 * */
typedef XuiGraphicsContext *(*XuiGraphicsContextCreate) (XwWindow *window);

/**
 * @b Destroy the given graphics context.
 *
 * @param gctx GraphicsContext object to be destroyed.
 * */
typedef void (*XuiGraphicsContextDestroy) (XuiGraphicsContext *gctx);

/**
 * @b Resize the graphics context.
 *
 * Some plugins need to be informed about when window is resized.
 * This call will ask the plugins to process a resize event from the given
 * graphics context associated with given xwin.
 *
 * @param graphics_context Graphics context to be resized.
 * @param xwin Window that was resized.
 *
 * @return @c True on success.
 * @return @c False otherwise.
 * */
typedef Bool (*XuiGraphicsContextResize) (XuiGraphicsContext *graphics_context, XwWindow *xwin);

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
typedef Bool (*XuiGraphicsDrawRect2D) (
    XuiGraphicsContext *graphics_context,
    XwWindow           *xwin,
    Rect2D              rect,
    Color               color
);

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_H
