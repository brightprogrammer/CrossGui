/**
 * @file Graphics.h
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_GRAPHICS_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_GRAPHICS_H

#include <Anvie/CrossGui/Plugin/Graphics/Api.h>

/**
 * @b Defines set of callbacks to be used to interact with the plugin.
 * */
typedef struct XuiGraphicsPlugin {
    /* graphics context methods */
    XuiGraphicsContextCreate  context_create;
    XuiGraphicsContextDestroy context_destroy;
    XuiGraphicsContextResize  context_resize;

    /* shape 2d methods */
    XuiShapeCreate2D     shape_create_2d;
    XuiShapeDestroy2D    shape_destroy_2d;
    XuiShapeReset2D      shape_reset_2d;
    XuiShapeReserve2D    shape_reserve_2d;
    XuiShapeAddVertices2D  shape_add_vertices_2d;
    XuiShapeIsComplete2D shape_is_complete_2d;

    /* drawing methods */
    XuiGraphicsDraw2D draw_2d;
    XuiGraphicsClear  clear;
} XuiGraphicsPlugin;

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_GRAPHICS_H
