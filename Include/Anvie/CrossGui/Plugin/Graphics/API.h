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
