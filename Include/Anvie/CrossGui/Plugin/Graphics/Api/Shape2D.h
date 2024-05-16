/**
 * @file Shape2D.h
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_SHAPE2D_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_SHAPE2D_H

#include <Anvie/Types.h>

/* crossgui */
#include <Anvie/CrossGui/Utils/Maths.h>

/**
 * @b The fill type allows the plugin to decide which rendering approach
 *    it must use. An example is that in vulkan the plugin can select different
 *    pipelines based on different fill types.
 * */
typedef enum XuiShapeFillType2D {
    XUI_SHAPE_FILL_TYPE_2D_UNKNOWN = 0,
    XUI_SHAPE_FILL_TYPE_2D_FILL, /**< @b Fill the shape with given color/texture data. */
    XUI_SHAPE_FILL_TYPE_2D_OUTLINE, /**< @b Just outline the shape treating pair of adjacent vertices as lines. */
    XUI_SHAPE_FILL_TYPE_2D_POINTS, /**< @b Just draw the vertices as points, no fill, no outline. */
    XUI_SHAPE_FILL_TYPE_2D_MAX
} XuiShapeFillType2D;

/**
 * @b Opaque structure defined inside the plugin.
 *
 * How every plugin manages shapes might be different from others.
 * This will be returned as a reference to the shape data that can be used
 * to communicate with respective plugin.
 * */
typedef struct XuiShape2D XuiShape2D;

/* TODO: 
 * - add documentation for these Api methods,
 * - define them in vulkan plugin
 * - create shape manager (create new shapes, add vertices, add shapes to queue to be sent to gpu)
 * - create dynamic vector to store shape data
 * - send shape data to gpu as uniform buffer jsut before draw if not sent already!
 * */
typedef XuiShape2D *(*XuiShapeCreate2D)();
typedef XuiShape2D *(*XuiShapeDestroy2D) (XuiShape2D *shape);
typedef XuiShape2D *(*XuiShapeReset2D) (XuiShape2D *shape);
typedef XuiShape2D *(*XuiShapeReserve2D) (XuiShape2D *shape, Size num_vertices);
typedef XuiShape2D *(*XuiShapeAddVertex2D) (XuiShape2D *shape, Vec2f *vertex);
typedef XuiShape2D *(*XuiShapeFinalize2D) (XuiShape2D *shape);

typedef struct XuiShapeInstance2D {
    XuiShape2D *shape;
    Vec4f       color;
    Vec2f       position;
    Vec2f       scale;
    Float32     depth;
} XuiShapeInstance2D;

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_SHAPE2D_H
