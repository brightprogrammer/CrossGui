/**
 * @file Graphics.h
 * @date Sat, 20st January 2024
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

#ifndef ANVIE_CROSSGUI_GRAPHICS_H
#define ANVIE_CROSSGUI_GRAPHICS_H

#include <Anvie/Types.h>

typedef struct Position {
    Float32 x;
    Float32 y;
    Float32 z;
} Position;

typedef struct Scale {
    Float32 l; /**< @b Scale length of shape by this factor. */
    Float32 h; /**< @b Scale height of shape by this factor. */
    Float32 b; /**< @b Scale breadth of shape by this factor. 2D shapes don't use this dimension. */
} Scale;

typedef struct Rect {
    Position position; /**< @b Rectangle position. */
    Scale    scale;    /**< @b Scale position. */
} Rect2D, Rect3D;

typedef struct Color {
    Float32 r;
    Float32 g;
    Float32 b;
    Float32 a;
} Color;

typedef struct Vertex {
    Position position;
    Color    color;
} Vertex2D;

#endif // ANVIE_CROSSGUI_GRAPHICS_H
