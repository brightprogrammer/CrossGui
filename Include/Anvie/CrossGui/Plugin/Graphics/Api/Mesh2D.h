/**
 * @file Mesh2D.h
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_MESH2D_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_MESH2D_H

#include <Anvie/Types.h>

/* crossgui */
#include <Anvie/CrossGui/Utils/Maths.h>

/**
 * @b Mesh2D is created by user code and given to the plugin to upload
 * the vertex data to the GPU. The user code then just needs the @c type
 * of shape to create mesh instances and all this data can be freed.
 * */
typedef struct XuiMesh2D {
    Uint32  type;         /**< @b A unique ID for each mesh 2D assigned by user code. */
    Vec2f  *vertices;     /**< @b Array of vertices for creating the mesh. */
    Uint32  vertex_count; /**< @b Number of vertices in @c mesh_vertices array. */
    Uint32 *indices;      /**< @b Array of indices for mesh. */
    Uint32  index_count;  /**< @b Number of indices in the @c mesh_indices array. */
} XuiMesh2D;

/**
 * @b Stores metadata about instance of selected mesh.
 * instance : mesh :: muscle : skeleton. 
 * */
typedef struct XuiMeshInstance2D {
    Uint32  type;     /**< @b Selected mesh. */
    Vec2f   scale;    /**< @b Scale factor of mesh instance. */
    Vec3f   position; /**< @b Position of mesh instance. */
    Vec4f   color;    /**< @b Color of mesh instance. */
} XuiMeshInstance2D;

typedef Bool (*XuiMeshUpload2D) (XuiMesh2D *mesh);

#endif // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_MESH2D_H
