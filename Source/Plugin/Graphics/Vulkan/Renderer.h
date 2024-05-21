/**
 * @file Renderer.h
 * @date Sat, 27th April 2024
 * @author Siddharth Mishra (admin@brightprograrendererer.in)
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

#ifndef ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H
#define ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H

#include <Anvie/Types.h>

/* crossgui-graphics-api */
#include <Anvie/CrossGui/Plugin/Graphics/Api/Common.h>

/* local includes */
#include "Device.h"
#include "RenderPass.h"

/* fwd declarations */
typedef struct XuiGraphicsContext XuiGraphicsContext;
typedef struct XwWindow           XwWindow;
typedef struct XuiMeshInstance2D  XuiMeshInstance2D;

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

/**
 * @b Batch Renderer works by creating and storing batches of multiple instances
 * of same mesh. A mesh instance is added whenever draw_Nd is called and all the batches
 * are rendered whenever display_Nd is called.
 * */
typedef struct BatchRenderer {
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

    RenderPass default_render_pass;
} BatchRenderer;

BatchRenderer *batch_renderer_init (BatchRenderer *renderer, Swapchain *swapchain);
BatchRenderer *batch_renderer_deinit (BatchRenderer *renderer);
MeshInstanceBatch2D *
    batch_renderer_get_mesh_instance_batch_by_type_2d (BatchRenderer *renderer, Uint32 type);
BatchRenderer *
    batch_renderer_add_mesh_instance_2d (BatchRenderer *renderer, XuiMeshInstance2D *mesh_instance);
BatchRenderer  *batch_renderer_reset_batches_2d (BatchRenderer *renderer);
BatchRenderer  *batch_renderer_upload_batches_to_gpu_2d (BatchRenderer *renderer);
XuiRenderStatus batch_renderer_draw_2d (BatchRenderer *renderer, XuiMeshInstance2D *mesh_instance);
XuiRenderStatus
    batch_renderer_display (BatchRenderer *renderer, Swapchain *swapchain, XwWindow *win);
XuiRenderStatus batch_renderer_clear (BatchRenderer *rederer, Swapchain *swapchain, XwWindow *win);

XuiRenderStatus gfx_draw_2d (XuiGraphicsContext *gctx, XuiMeshInstance2D *mesh_instance);
XuiRenderStatus gfx_display (XuiGraphicsContext *gctx, XwWindow *win);
XuiRenderStatus gfx_clear (XuiGraphicsContext *gctx, XwWindow *win);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_RENDERER_H
