/**
 * @file MeshManager.c
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


#include <Anvie/Common.h>

/* libc */
#include <memory.h>

/* crossgui-graphics-api */
#include <Anvie/CrossGui/Plugin/Graphics/Api/Mesh2D.h>

/* crossgui-utils */
#include <Anvie/CrossGui/Utils/Vector.h>
#include <vulkan/vulkan_core.h>

/* local includes */
#include "Device.h"
#include "MeshManager.h"
#include "Vulkan.h"

NEW_VECTOR_TYPE (MeshData2D, mesh_data_2d);
NEW_VECTOR_TYPE (MeshInstanceBatch2D, mesh_instance_batch_2d);
NEW_VECTOR_TYPE (XuiMeshInstance2D, mesh_instance_2d);

/**************************************************************************************************/
/***************************** MESH INSTANCE BATCH 2D PUBLIC METHODS ******************************/
/**************************************************************************************************/

MeshInstanceBatch2D *mesh_instance_batch_init_2d (MeshInstanceBatch2D *batch, Uint32 type) {
    RETURN_VALUE_IF (!batch, Null, ERR_INVALID_ARGUMENTS);

    /* create vector */
    RETURN_VALUE_IF (
        !(batch->instances.data = mesh_instance_2d_vector_create (16, &batch->instances.capacity)),
        Null,
        "Failed to create vector to store batch of mesh instances 2D.\n"
    );

    /* set type */
    batch->mesh_type = type;

    return batch;
}

MeshInstanceBatch2D *mesh_instance_batch_deinit_2d (MeshInstanceBatch2D *batch) {
    RETURN_VALUE_IF (!batch, Null, ERR_INVALID_ARGUMENTS);

    mesh_instance_2d_vector_destroy (batch->instances.data);
    memset (batch, 0, sizeof (MeshInstanceBatch2D));

    return batch;
}

MeshInstanceBatch2D *mesh_instance_batch_add_instance_2d (
    MeshInstanceBatch2D *batch,
    XuiMeshInstance2D   *mesh_instance
) {
    RETURN_VALUE_IF (!batch || !mesh_instance, Null, ERR_INVALID_ARGUMENTS);

    /* resize if required */
    if (batch->instances.count >= batch->instances.capacity) {
        Size               newcap = 0;
        XuiMeshInstance2D *tmpbuf = Null;
        RETURN_VALUE_IF (
            !(tmpbuf = mesh_instance_2d_vector_resize (
                  batch->instances.data,
                  batch->instances.count,     /* from count */
                  batch->instances.count + 1, /* to count */
                  batch->instances.capacity,  /* from cap */
                  &newcap                     /* to new cap (automatically set by the function) */
              )),
            Null,
            "Failed to resize vector to store more mesh instance data in corresponding batch\n"
        );

        batch->instances.data     = tmpbuf;
        batch->instances.capacity = newcap;
    }

    batch->instances.data[batch->instances.count++] = *mesh_instance;

    return batch;
}

MeshInstanceBatch2D *mesh_instance_batch_2d_reset (MeshInstanceBatch2D *batch) {
    RETURN_VALUE_IF (!batch, Null, ERR_INVALID_ARGUMENTS);

    /* for now and probably forever, reset just means this 
     * until unless we enter some crazy memory optimization and we cap memory of
     * vectors after a reset */
    batch->instances.count = 0;

    return batch;
}

/**************************************************************************************************/
/********************************** MESH MANAGER PUBLIC METHODS ***********************************/
/**************************************************************************************************/

MeshManager *mesh_manager_init (MeshManager *mm) {
    RETURN_VALUE_IF (!mm, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        !(mm->mesh_data_2d.data = mesh_data_2d_vector_create (256, &mm->mesh_data_2d.capacity)),
        Null,
        "Failed to create vector to store mesh"
    );

    /* TODO: Create a batch renderer and move all these mesh instances to it.
     * keep the meshes in mesh manager as they're independent of window 
     * BatchRenderer must be tightly coupled with GraphicsContext because instances are tied to a window */

    RETURN_VALUE_IF (
        !(mm->batches_2d.data =
              mesh_instance_batch_2d_vector_create (128, &mm->batches_2d.capacity)),
        Null,
        "Failed to create vector to store batches"
    );

    return mm;
}

MeshManager *mesh_manager_deinit (MeshManager *mm) {
    RETURN_VALUE_IF (!mm, Null, ERR_INVALID_ARGUMENTS);

    if (mm->mesh_data_2d.data) {
        for (Size s = 0; s < mm->mesh_data_2d.count; s++) {
            device_buffer_deinit (&mm->mesh_data_2d.data[s].vertex);
            device_buffer_deinit (&mm->mesh_data_2d.data[s].index);
        }

        mesh_data_2d_vector_destroy (mm->mesh_data_2d.data);
    }

    if (mm->batches_2d.data) {
        /* since in a batch reset we don't free the memory, it's possible
         * that some batch exceed the current count boundary is left allocated,
         * we need to free those as well, so we instead search the complete array of left out memory */
        for (Size s = 0; s < mm->batches_2d.capacity; s++) {
            /* if batch is not freed then free it */
            if (mm->batches_2d.data[s].instances.data) {
                mesh_instance_batch_deinit_2d (mm->batches_2d.data + s);
            }
        }

        mesh_instance_batch_2d_vector_destroy (mm->batches_2d.data);
    }

    memset (mm, 0, sizeof (MeshManager));
    return mm;
}

MeshManager *mesh_manager_upload_mesh_2d (MeshManager *mm, XuiMesh2D *mesh) {
    RETURN_VALUE_IF (!mm || !mesh, Null, ERR_INVALID_ARGUMENTS);

    /* resize if required */
    if (mm->mesh_data_2d.count >= mm->mesh_data_2d.capacity) {
        Size        newcap = 0;
        MeshData2D *tmpbuf = Null;
        RETURN_VALUE_IF (
            !(tmpbuf = mesh_data_2d_vector_resize (
                  mm->mesh_data_2d.data,
                  mm->mesh_data_2d.count,     /* from count */
                  mm->mesh_data_2d.count + 1, /* to count */
                  mm->mesh_data_2d.capacity,  /* from cap */
                  &newcap                     /* to new cap (automatically set by the function) */
              )),
            Null,
            "Failed to resize vector to store more mesh data\n"
        );

        mm->mesh_data_2d.data     = tmpbuf;
        mm->mesh_data_2d.capacity = newcap;
    }

    MeshData2D new_data =
        {.type = mesh->type, .vertex_count = mesh->vertex_count, .index_count = mesh->index_count};

    /* setup new mesh data */
    {
        RETURN_VALUE_IF (
            !device_buffer_init (
                &new_data.vertex,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                mesh->vertex_count * sizeof (*mesh->vertices),
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                vk.device.graphics_queue.family_index
            ),
            Null,
            "Failed to create VBO for storing mesh vertex data"
        );

        RETURN_VALUE_IF (
            !device_buffer_init (
                &new_data.index,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                mesh->index_count * sizeof (*mesh->indices),
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                vk.device.graphics_queue.family_index
            ),
            Null,
            "Failed to create IBO for storing mesh index data"
        );

        RETURN_VALUE_IF (
            !device_buffer_memcpy (
                &new_data.vertex,
                mesh->vertices,
                mesh->vertex_count * sizeof (*mesh->vertices)
            ),
            Null,
            "Failed to upload vertex data to GPU VBO\n"
        );

        RETURN_VALUE_IF (
            !device_buffer_memcpy (
                &new_data.index,
                mesh->indices,
                mesh->index_count * sizeof (*mesh->indices)
            ),
            Null,
            "Failed to upload vertex data to GPU VBO\n"
        );
    }

    /* insert data */
    mm->mesh_data_2d.data[mm->mesh_data_2d.count++] = new_data;

    return mm;
}

MeshData2D *mesh_manager_get_mesh_data_by_type_2d (MeshManager *mm, Uint32 type) {
    RETURN_VALUE_IF (!mm, Null, ERR_INVALID_ARGUMENTS);

    /* TODO: find a way to convert this linear search to binary search */
    for (Size s = 0; s < mm->mesh_data_2d.count; s++) {
        if (mm->mesh_data_2d.data[s].type == type) {
            return mm->mesh_data_2d.data + s;
        }
    }

    return Null;
}

MeshInstanceBatch2D *
    mesh_manager_get_mesh_instance_batch_by_type_2d (MeshManager *mm, Uint32 type) {
    RETURN_VALUE_IF (!mm, Null, ERR_INVALID_ARGUMENTS);

    for (Size s = 0; s < mm->batches_2d.count; s++) {
        if (mm->batches_2d.data[s].mesh_type == type) {
            return mm->batches_2d.data + s;
        }
    }

    return Null;
}

MeshManager *mesh_manager_add_mesh_instance_2d (MeshManager *mm, XuiMeshInstance2D *mesh_instance) {
    RETURN_VALUE_IF (!mm || !mesh_instance, Null, ERR_INVALID_ARGUMENTS);

    /* find batch */
    MeshInstanceBatch2D *batch =
        mesh_manager_get_mesh_instance_batch_by_type_2d (mm, mesh_instance->type);

    /* create batch if not already created */
    if (!batch) {
        /* resize if required */
        if (mm->batches_2d.count >= mm->batches_2d.capacity) {
            Size                 newcap = 0;
            MeshInstanceBatch2D *tmpbuf = Null;
            RETURN_VALUE_IF (
                !(tmpbuf = mesh_instance_batch_2d_vector_resize (
                      mm->batches_2d.data,
                      mm->batches_2d.count,     /* from count */
                      mm->batches_2d.count + 1, /* to count */
                      mm->batches_2d.capacity,  /* from cap */
                      &newcap                   /* to new cap (automatically set by the function) */
                  )),
                Null,
                "Failed to resize vector to batches\n"
            );

            mm->batches_2d.data     = tmpbuf;
            mm->batches_2d.capacity = newcap;
        }

        batch = mm->batches_2d.data + mm->batches_2d.count++;
        mesh_instance_batch_init_2d (batch, mesh_instance->type);
    }

    /* insert mesh instance to corresponding batch */
    mesh_instance_batch_add_instance_2d (batch, mesh_instance);

    return mm;
}

MeshManager *mesh_manager_reset_batches_2d (MeshManager *mm) {
    RETURN_VALUE_IF (!mm, Null, ERR_INVALID_ARGUMENTS);

    for (Size s = 0; s < mm->batches_2d.count; s++) {
        mesh_instance_batch_2d_reset (mm->batches_2d.data + s);
    }

    return mm;
}
