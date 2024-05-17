/**
 * @file Shape2D.c
 * @date Fri, 17th May 2024
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

/* crossgui/utils */
#include <Anvie/CrossGui/Utils/Maths.h>
#include <Anvie/CrossGui/Utils/Vector.h>

/* libc */
#include <memory.h>

/* local includes */
#include "Shape2D.h"

NEW_VECTOR_TYPE (Vec2f, vec2f);

/**
 * @b Ask the plugin to create a new shape 2d object.
 *
 * @return @c XuiShape2D on success.
 * @return @c Null otherwise.
 * */
XuiShape2D *shape_create_2d() {
    XuiShape2D *shape = NEW (XuiShape2D);
    RETURN_VALUE_IF (!shape, Null, ERR_OUT_OF_MEMORY);

    shape->vertex_count = 4;
    RETURN_VALUE_IF (
        !(shape->vertices = vec2f_vector_create (shape->vertex_count, &shape->vertex_capacity)),
        Null,
        "Failed to create vector to store shape data.\n"
    );

    return shape;
}

/**
 * @b Destroy given shape.
 *
 * @param shape.
 * */
void shape_destroy_2d (XuiShape2D *shape) {
    RETURN_IF (!shape, Null, ERR_INVALID_ARGUMENTS);

    if (shape->vertices) {
        vec2f_vector_destroy (shape->vertices);
        shape->vertices = Null;
    }

    shape->vertex_count    = 0;
    shape->vertex_capacity = 0;
    shape->is_complete     = False;

    FREE (shape);
}

/**
 * @b Reset given shape.
 *
 * The plugin instead of freeing up, or performing any actual memory operations,
 * would just set the current size to 0. This will create an illision for other
 * methods that the array is empty, but it's not.
 * 
 * @b shape
 *
 * @return @c shape on success.
 * @return @c Null otherwise.
 * */
XuiShape2D *shape_reset_2d (XuiShape2D *shape) {
    RETURN_VALUE_IF (!shape, Null, ERR_INVALID_ARGUMENTS);

    shape->vertex_count = 0;
    shape->is_complete  = False;
    return shape;
}

/**
 * @b Increase the capacity of the vector (the total number of elements that the vector
 *    can hold without requiring reallocation) to a value that's greater or equal to
 *    new_cap. If new_cap is greater than the current capacity(), new storage is allocated,
 *    otherwise the function does nothing.
 *
 * @param shape
 * @param num_vertices New capacity of vertices array.
 *
 * @return @c shape on success.
 * @return @c Null otherwise.
 * */
XuiShape2D *shape_reserve_2d (XuiShape2D *shape, Size num_vertices) {
    RETURN_VALUE_IF (!shape, Null, ERR_INVALID_ARGUMENTS);

    if (shape->is_complete) {
        PRINT_ERR (
            "Reserving more space for a completed shape? This might be a bug in the application\n"
        );
    }

    if (num_vertices <= shape->vertex_capacity) {
        return shape;
    }

    Vec2f *tmp    = Null;
    Size   newcap = 0;
    RETURN_VALUE_IF (
        !(tmp = vec2f_vector_resize (
              shape->vertices,
              shape->vertex_count,    /* from count */
              num_vertices,           /* to count */
              shape->vertex_capacity, /* from capacity */
              &newcap                 /* to capacity */
          )),
        Null,
        "Failed to resize vertex array.\n"
    );

    shape->vertices        = tmp;
    shape->vertex_capacity = newcap;

    return shape;
}

/**
 * @b Add contents of given point/vertex (@c Vec2f) to this shape.
 *
 * @param shape
 * @param vertices Array of vertices to be added to shape.
 * @param num_vertices @c Number of vertices in the @c vertices array.
 *
 * @return @c shape on success.
 * @return @c Null otherwise.
 * */
XuiShape2D *shape_add_vertices_2d (XuiShape2D *shape, Vec2f *vertices, Size num_vertices) {
    RETURN_VALUE_IF (!shape || !vertices || !num_vertices, Null, ERR_INVALID_ARGUMENTS);
    RETURN_VALUE_IF (
        shape->is_complete,
        Null,
        "Cannot add more vertices to a completed shape. Reset and re-add.\n"
    );

    /* resize if required */
    if (shape->vertex_count + num_vertices > shape->vertex_capacity) {
        Vec2f *tmp    = Null;
        Size   newcap = 0;
        RETURN_VALUE_IF (
            !(tmp = vec2f_vector_resize (
                  shape->vertices,
                  shape->vertex_count,                /* from count */
                  shape->vertex_count + num_vertices, /* to count */
                  shape->vertex_capacity,             /* from capacity */
                  &newcap                             /* to capacity */
              )),
            Null,
            "Failed to resize vertex array.\n"
        );

        shape->vertices        = tmp;
        shape->vertex_capacity = newcap;
    }

    /* add vertices */
    memcpy (shape->vertices + shape->vertex_count, vertices, num_vertices);
    shape->vertex_count += num_vertices;

    return shape;
}

/**
 * @b Mark the shape as complete.
 *
 * @param shape.
 *
 * @return @c shape on success.
 * @return @c Null otherwise.
 * */
XuiShape2D *shape_is_complete_2d (XuiShape2D *shape) {
    RETURN_VALUE_IF (!shape, Null, ERR_INVALID_ARGUMENTS);

    shape->is_complete = True;

    return shape;
}
