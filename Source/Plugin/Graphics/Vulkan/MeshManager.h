/**
 * @file MeshManager.h
 * @date Sun, 19th May 2024
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

#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_MESH_MANAGER_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_MESH_MANAGER_H

#include <Anvie/Types.h>

/* local inclueds */
#include "Anvie/Common.h"
#include "Device.h"

/* fwd-declaration */
typedef struct XuiMesh2D         XuiMesh2D;
typedef struct XuiMeshInstance2D XuiMeshInstance2D;

typedef struct MeshData2D {
    Uint32       type; /**< @b A unique ID assigned to each mesh by the user code. */
    DeviceBuffer vertex;
    Size         vertex_count;
    DeviceBuffer index;
    Size         index_count;
} MeshData2D;

/**
 * @b A batch is made by grouping together all mesh instances that belong to
 * a certain mesh type. The mesh manager then creates an array of these batches,
 * corresponding to each mesh type.
 * */
typedef struct MeshInstanceBatch2D {
    /**
     * @b To what instance type does this batch belong to.
     * */
    Uint32 mesh_type;

    /**
     * @b Vector of mesh instances corresponding to this batch.
     * */
    struct {
        Size               count;
        Size               capacity;
        XuiMeshInstance2D *data;
    } instances;

    DeviceBuffer device_data;
} MeshInstanceBatch2D;

MeshInstanceBatch2D *mesh_instance_batch_init_2d (MeshInstanceBatch2D *batch, Uint32 type);
MeshInstanceBatch2D *mesh_instance_batch_deinit_2d (MeshInstanceBatch2D *batch);
MeshInstanceBatch2D *mesh_instance_batch_add_instance_2d (
    MeshInstanceBatch2D *batch,
    XuiMeshInstance2D   *mesh_instance
);
MeshInstanceBatch2D *mesh_instance_batch_reset_2d (MeshInstanceBatch2D *batch);
MeshInstanceBatch2D *mesh_instance_batch_upload_to_gpu_2d (MeshInstanceBatch2D *batch);

typedef struct MeshManager {
    /**
     * @b Mesh data for each mesh type.
     * Mesh data added to this vector stays as long as the application is
     * running (plugin is in use and is kept loaded)
     * */
    struct {
        Size        count;
        Size        capacity;
        MeshData2D *data;
    } mesh_data_2d;

    /**
     * @b Vector storing batches corresponding to each mesh type.
     * Mesh instance data keeps getting added and removed quite frequently,
     * based on how many times the user issues a gfx_reset, which consequently
     * resets the contents of this vector.
     * */
    struct {
        Size                 count;
        Size                 capacity;
        MeshInstanceBatch2D *data;
    } batches_2d;
} MeshManager;

MeshManager         *mesh_manager_init (MeshManager *mm);
MeshManager         *mesh_manager_deinit (MeshManager *mm);
MeshManager         *mesh_manager_upload_mesh_2d (MeshManager *mm, XuiMesh2D *mesh);
MeshData2D          *mesh_manager_get_mesh_data_by_type_2d (MeshManager *mm, Uint32 type);
MeshInstanceBatch2D *mesh_manager_get_mesh_instance_batch_by_type_2d (MeshManager *mm, Uint32 type);
MeshManager *mesh_manager_add_mesh_instance_2d (MeshManager *mm, XuiMeshInstance2D *mesh_instance);
MeshManager *mesh_manager_reset_batches_2d (MeshManager *mm);
MeshManager *mesh_manager_upload_batches_to_gpu_2d(MeshManager* mm);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_MESH_MANAGER_H
