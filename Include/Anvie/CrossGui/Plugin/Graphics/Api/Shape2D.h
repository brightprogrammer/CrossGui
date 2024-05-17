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
 *    it must use.
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
 * - define them in vulkan plugin
 * - create shape manager (create new shapes, add vertices, add shapes to queue to be sent to gpu)
 * - create dynamic vector to store shape data
 * - send shape data to gpu as uniform buffer jsut before draw if not sent already!
 * */

/**
 * @b Ask the plugin to create a new shape 2d object.
 *
 * @return @c XuiShape2D on success.
 * @return @c Null otherwise.
 * */
typedef XuiShape2D *(*XuiShapeCreate2D)();

/**
 * @b Destroy given shape object. This will also remove the shape
 *    data from the GPU.
 *
 * @param shape
 * */
typedef void (*XuiShapeDestroy2D) (XuiShape2D *shape);

/**
 * @b Reset the shape vertex data. 
 *
 * Once a reset is done on a shape, all it's current shape data will be lost.
 * This is usually done when one needs to change the shape data. Resetting may
 * or may not free the allocated memory as this depends on the plugin.
 *
 * @param shape.
 * 
 * @return @c XuiShape2D on success.
 * @return @c Null otherwise.
 * */
typedef XuiShape2D *(*XuiShapeReset2D) (XuiShape2D *shape);

/**
 * @b Reserve space for given number of vertices. Recommended
 *    if number of vertices is already known.
 *
 * @param shape
 * @param num_vertices
 *
 * @return @c XuiShape2D on success.
 * @return @c XuiShape2D otherwise.
 * */
typedef XuiShape2D *(*XuiShapeReserve2D) (XuiShape2D *shape, Size num_vertices);

/**
 * @b Add a new vertex to given shape.
 *
 * @param shape
 * @param vertex
 *
 * @return @c XuiShape2D on success.
 * @return @c Null otherwise.
 * */
typedef XuiShape2D *(*XuiShapeAddVertices2D) (XuiShape2D *shape, Vec2f *vertices, Size num_vertices);

/**
 * @b Call this when the shape is complete, meaning no vertices need to be added.
 * 
 * Once a shape is complete, the plugin may add it to a queue containing shapes
 * that are ready to be sent to GPU. The shape may also be made available to the
 * GPU the moment an `IsComplete` call is issued. Both cases don't affect the
 * availability of the shape inside a draw call as it's guaranteed that all 
 * completed shapes will always be available.
 *
 * @param shape.
 *
 * @return @c XuiShape2D on success.
 * @return @c Null otherwise.
 * */
typedef XuiShape2D *(*XuiShapeIsComplete2D) (XuiShape2D *shape);

/**
 * @b Stores metadata about instance of selected shape. An instance of
 *    a shape is to shape as skin & muscles is to skeleton.
 * */
typedef struct XuiShapeInstance2D {
    XuiShape2D        *shape;     /**< @b Selected shape. */
    Vec4f              color;     /**< @b Color of shape instance. */
    Vec3f              position;  /**< @b Position of shape instance. */
    Vec2f              scale;     /**< @b Scale factor of shape instance. */
    XuiShapeFillType2D fill_type; /**< @b fill/outline/points. */
} XuiShapeInstance2D;

#endif                            // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_SHAPE2D_H
