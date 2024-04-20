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

typedef struct Graphics Graphics;

typedef struct Position2D {
    Float32 x;
    Float32 y;
} Position2D;

typedef struct Color {
    Float32 r;
    Float32 g;
    Float32 b;
    Float32 a;
} Color;

typedef struct Vertex2D {
    Position2D position;
    Color      color;
} Vertex2D;

/**
 * @b Plugin must render given 2D shape.
 *
 * This is a per-vertex draw call.  
 *
 * @param vertices Array of 2D vertices.
 * @param vertex_count Number of vertices.
 *
 * @return True if draw was successful.
 * @return False otherwise.
 *
 * @sa XuiGraphicsDrawIndexed2D
 * */
typedef Bool (*XuiGraphicsDraw2D) (Vertex2D *vertices, Size vertex_count);

/**
 * @b Plugin must render given 2D shape, along with indices data.
 *
 * This is an indexed, per-vertex call. Indices define the actual shape.
 * Refer to indexed drawing method for more detail.
 *
 * @param vertices Array of 2D vertices.
 * @param vertex_count Number of vertices.
 *
 * @return True if draw was successful.
 * @return False otherwise.
 *
 * @sa XuiGraphicsDrawIndexed2D
 * */
typedef Bool (*XuiGraphicsDrawIndexed2D) (
    Vertex2D *vertices,
    Size      vertex_count,
    Uint32    indices,
    Size      index_count
);

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_H
