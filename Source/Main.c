#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* crosswindow */
#include <Anvie/CrossWindow/Event.h>
#include <Anvie/CrossWindow/Vulkan.h>
#include <Anvie/CrossWindow/Window.h>

/* crossgui */
#include <Anvie/CrossGui/Graphics.h>
#include <Anvie/CrossGui/Utils/Vector.h>

/* libc */
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

// TODO(brightprogrammer): implement draw_2d and draw_indexed_2d methods in Vulkan Graphics Plugin

// NOTE : Create a single unifotm data to store UI mesh data.
// This will include things like mesh data of button, rounded or plain, or circular, etc...
// This uniform is a global uniform and will always be bound. At top of the hierarchy of
// shader resource bindings.
// Use GpuUiData for passing all these data
typedef struct GpuUiData {
    Float32 button_width;  /**< @b Scale width of all buttons by this factor */
    Float32 button_height; /**< @b Scale height of all buttons by this factor */
} GpuUiData;

/**************************************************************************************************/
/**************************************** SHADER RESOURCE *****************************************/
/**************************************************************************************************/

typedef struct ShaderResourceBinding {
    VkDescriptorPool      descriptor_pool;
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_set;

    DeviceBuffer *uniform_buffer; /**< @b Uniform buffer to be sent to shader */
} ShaderResourceBinding;

ShaderResourceBinding *shader_resource_binding_create_ui_binding (Device *device);
void                   shader_resource_binding_destroy (ShaderResourceBinding *srb, Device *device);

typedef struct ShaderPipeline {
    VkPipelineLayout pipeline_layout;
    VkPipeline       pipeline;
} ShaderPipeline;

ShaderPipeline *shader_pipeline_create_ui_pipeline (
    Device                *device,
    Surface               *surface,
    ShaderResourceBinding *srb
);
void shader_pipeline_destroy (ShaderPipeline *pipeline, Device *device);

/**************************************************************************************************/
/********************************************** Math **********************************************/
/**************************************************************************************************/

static const Vertex2D triangle_vertices[] = {
    {{-1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
    {  {1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},
    { {-1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},

    {  {1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},
    {{-1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
    { {1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
};

/**************************************************************************************************/
/************************************************  ************************************************/
/**************************************************************************************************/

int main() {
    const Uint32 width  = 360;
    const Uint32 height = 240;
    XwWindow    *win    = xw_window_create ("Ckeckl", width, height, 10, 20);

    Vulkan *vk = vk_create();
    GOTO_HANDLER_IF (!vk, VK_INIT_FAILED, "Failed to create Vulkan\n");

    Device *device = device_create (vk);
    GOTO_HANDLER_IF (!device, DEVICE_FAILED, "Failed to create Device\n");

    Surface *surface = surface_create (vk, device, win);
    GOTO_HANDLER_IF (!surface, SURFACE_FAILED, "Failed to create Surface\n");

    DeviceBuffer *vbo = device_buffer_create (
        device,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        sizeof (triangle_vertices),
        device->graphics_queue.family_index
    );
    GOTO_HANDLER_IF (!vbo, DEVICE_BUFFER_FAILED, "Failed to create DeviceBuffer\n");

    device_buffer_memcpy (vbo, device, (void *)triangle_vertices, sizeof (triangle_vertices));

    ShaderResourceBinding *ui_srb = shader_resource_binding_create_ui_binding (device);
    GOTO_HANDLER_IF (!ui_srb, UI_SRB_FAILED, "Failed to create UI SRB\n");

    ShaderPipeline *ui_pipeline = shader_pipeline_create_ui_pipeline (device, surface, ui_srb);
    GOTO_HANDLER_IF (!ui_pipeline, UI_PIPELINE_FAILED, "Failed to create UI Pipeline");

    GpuUiData ui_data = {.button_width = 0.25f, .button_height = 0.10f};
    device_buffer_memcpy (ui_srb->uniform_buffer, device, &ui_data, sizeof (GpuUiData));

    /* event handlign looop */
    Bool    is_running = True;
    XwEvent e;
    while (is_running) {
        Bool resized = False;
        while (xw_event_poll (&e)) {
            switch (e.type) {
                case XW_EVENT_TYPE_CLOSE_WINDOW : {
                    is_running = False;
                    break;
                }
                case XW_EVENT_TYPE_RESIZE : {
                    resized = True;
                    break;
                }
                default :
                    break;
            }
        }

    }

    shader_pipeline_destroy (ui_pipeline, device);
    shader_resource_binding_destroy (ui_srb, device);
    device_buffer_destroy (vbo, device);
    surface_destroy (surface, device, vk);
    device_destroy (device);
    vk_destroy (vk);
    xw_window_destroy (win);

    return EXIT_SUCCESS;

DRAW_ERROR:
    shader_pipeline_destroy (ui_pipeline, device);
UI_PIPELINE_FAILED:
    shader_resource_binding_destroy (ui_srb, device);
UI_SRB_FAILED:
    device_buffer_destroy (vbo, device);
DEVICE_BUFFER_FAILED:
    surface_destroy (surface, device, vk);
SURFACE_FAILED:
    device_destroy (device);
DEVICE_FAILED:
    vk_destroy (vk);
VK_INIT_FAILED:
    xw_window_destroy (win);
    return EXIT_FAILURE;
}

/**************************************************************************************************/
/********************************** PRIVATE METHOD DELCARATIONS ***********************************/
/**************************************************************************************************/

/* private helper methods */
static inline VkShaderModule load_shader (VkDevice device, CString path);

/**************************************************************************************************/
/*********************** SHADER RESOURCE BINDING PUBLIC METHOD DEFINITIONS ************************/
/**************************************************************************************************/

ShaderResourceBinding *shader_resource_binding_create_ui_binding (Device *device) {
    RETURN_VALUE_IF (!device, Null, ERR_INVALID_ARGUMENTS);

    ShaderResourceBinding *srb = NEW (ShaderResourceBinding);
    RETURN_VALUE_IF (!srb, Null, ERR_OUT_OF_MEMORY);

    /* create descriptor pool */
    {
        VkDescriptorPoolSize pool_sizes[] = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1}
        };

        VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext         = Null,
            .flags         = 0,
            .maxSets       = 1,
            .poolSizeCount = ARRAY_SIZE (pool_sizes),
            .pPoolSizes    = pool_sizes
        };

        VkResult res = vkCreateDescriptorPool (
            device->device,
            &descriptor_pool_create_info,
            Null,
            &srb->descriptor_pool
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            POOL_FAILED,
            "Failed to create descriptor set. RET = %d\n",
            res
        );
    }

    /* describe descriptor set layout */
    {
        VkDescriptorSetLayoutBinding bindings[] = {
            {.binding            = 0,
             .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount    = 1,
             .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
             .pImmutableSamplers = Null}
        };

        VkDescriptorSetLayoutCreateInfo set_layout_create_info = {
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext        = Null,
            .flags        = 0,
            .bindingCount = ARRAY_SIZE (bindings),
            .pBindings    = bindings
        };

        VkResult res = vkCreateDescriptorSetLayout (
            device->device,
            &set_layout_create_info,
            Null,
            &srb->descriptor_set_layout
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            SET_LAYOUT_FAILED,
            "Failed to create descriptor set layout. RET = %d\n",
            res
        );
    }

    /* allocate one single descriptor set */
    {
        VkDescriptorSetAllocateInfo set_allocate_info = {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = Null,
            .descriptorPool     = srb->descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts        = (VkDescriptorSetLayout[]) {srb->descriptor_set_layout}
        };

        VkResult res =
            vkAllocateDescriptorSets (device->device, &set_allocate_info, &srb->descriptor_set);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            SET_ALLOC_FAILED,
            "Failed to create descriptor set layout. RET = %d\n",
            res
        );
    }

    /* create uniform buffer to send GPU data */
    {
        srb->uniform_buffer = device_buffer_create (
            device,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            sizeof (GpuUiData),
            device->graphics_queue.family_index
        );

        GOTO_HANDLER_IF (
            !srb->uniform_buffer,
            UNIFORM_FAILED,
            "Failed to create a uniform buffer for passing UI data to GPU\n"
        );
    }

    /* update descriptor set by writing to one */
    {
        VkDescriptorBufferInfo buffer_info = {
            .buffer = srb->uniform_buffer->buffer,
            .range  = srb->uniform_buffer->size,
            .offset = 0
        };

        VkWriteDescriptorSet write_descriptor_set = {
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = Null,
            .dstSet           = srb->descriptor_set,
            .dstBinding       = 0,
            .dstArrayElement  = 0,
            .descriptorCount  = 1,
            .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo       = Null,
            .pBufferInfo      = &buffer_info,
            .pTexelBufferView = Null
        };

        vkUpdateDescriptorSets (device->device, 1, &write_descriptor_set, 0, Null);
    }

    return srb;

UNIFORM_FAILED:
    /* follwing down this will automatically free the descriptor set */
SET_ALLOC_FAILED:
    vkDestroyDescriptorSetLayout (device->device, srb->descriptor_set_layout, Null);
SET_LAYOUT_FAILED:
    vkDestroyDescriptorPool (device->device, srb->descriptor_pool, Null);
POOL_FAILED:
    FREE (srb);
    return Null;
}

/**
 * @b Destroy given ShaderResourceBinding object.
 *
 * @param srb 
 * @param device
 * */
void shader_resource_binding_destroy (ShaderResourceBinding *srb, Device *device) {
    RETURN_IF (!srb || !device, ERR_INVALID_ARGUMENTS);

    vkDeviceWaitIdle (device->device);

    if (srb->uniform_buffer) {
        device_buffer_destroy (srb->uniform_buffer, device);
        srb->uniform_buffer = Null;
    }

    if (srb->descriptor_set_layout) {
        vkDestroyDescriptorSetLayout (device->device, srb->descriptor_set_layout, Null);
        srb->descriptor_set_layout = VK_NULL_HANDLE;
    }

    if (srb->descriptor_pool) {
        vkDestroyDescriptorPool (device->device, srb->descriptor_pool, Null);
        srb->descriptor_pool = VK_NULL_HANDLE;
        srb->descriptor_set  = VK_NULL_HANDLE;
    }

    FREE (srb);
}

/**
 * @b Create graphics pipeline and store it in given Surface.
 *
 * @param surface
 *
 * @return @c surface on success.
 * @return @c Null otherwise.
 * */
ShaderPipeline *shader_pipeline_create_ui_pipeline (
    Device                *device,
    Surface               *surface,
    ShaderResourceBinding *srb
) {
    RETURN_VALUE_IF (!device || !srb || !surface, Null, ERR_INVALID_ARGUMENTS);

    ShaderPipeline *pipeline = NEW (ShaderPipeline);
    RETURN_VALUE_IF (!pipeline, Null, ERR_OUT_OF_MEMORY);

    /* create a pipeline layout including provided shader resource binding */
    {
        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext                  = Null,
            .flags                  = 0,
            .setLayoutCount         = 1,
            .pSetLayouts            = (VkDescriptorSetLayout[]) {srb->descriptor_set_layout},
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = Null
        };

        VkResult res = vkCreatePipelineLayout (
            device->device,
            &pipeline_layout_create_info,
            Null,
            &pipeline->pipeline_layout
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            PIPELINE_LAYOUT_FAILED,
            "Failed to create pipeline layout. RET = %d\n",
            res
        );
    }

    /* create pipeline */
    VkShaderModule vert_shader = VK_NULL_HANDLE, frag_shader = VK_NULL_HANDLE;
    {
        vert_shader = load_shader (device->device, "bin/Shaders/triangle.vert.spv");
        frag_shader = load_shader (device->device, "bin/Shaders/triangle.frag.spv");
        GOTO_HANDLER_IF (
            !vert_shader || !frag_shader,
            SHADER_LOAD_FAILED,
            "Failed to load vertex/fragment shaders\n"
        );

        /* add shader modules and shader stages to be used in this pipeline here */
        struct {
            VkShaderModule     module;
            VkShaderStageFlags stage;
        } stage_infos[] = {
            {.module = vert_shader,   .stage = VK_SHADER_STAGE_VERTEX_BIT},
            {.module = frag_shader, .stage = VK_SHADER_STAGE_FRAGMENT_BIT}
        };

        /* create shader stages array */
        VkPipelineShaderStageCreateInfo shader_stages[ARRAY_SIZE (stage_infos)];
        for (Size s = 0; s < ARRAY_SIZE (shader_stages); s++) {
            shader_stages[s] = (VkPipelineShaderStageCreateInfo
            ) {.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
               .pNext               = Null,
               .flags               = 0,
               .stage               = stage_infos[s].stage,
               .module              = stage_infos[s].module,
               .pName               = "main",
               .pSpecializationInfo = Null};
        }

        /* describe how vertex data is sent to GPU */
        VkVertexInputBindingDescription vertex_binding_desc =
            {.binding = 0, .stride = sizeof (Vertex2D), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

        VkVertexInputAttributeDescription vertex_attribute_desc[] = {
            {.location = 0,
             .binding  = 0,
             .format   = VK_FORMAT_R32G32_SFLOAT,
             .offset   = offsetof (Vertex2D, position)},
            {.location = 1,
             .binding  = 0,
             .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset   = offsetof (Vertex2D,    color)},
        };

        /* describe vertex input state */
        VkPipelineVertexInputStateCreateInfo vertex_input_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = Null,
            .flags = 0,
            .vertexBindingDescriptionCount   = 1,
            .pVertexBindingDescriptions      = &vertex_binding_desc,
            .vertexAttributeDescriptionCount = ARRAY_SIZE (vertex_attribute_desc),
            .pVertexAttributeDescriptions    = vertex_attribute_desc
        };

        /* how to assemble input vertex data */
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {0};
        input_assembly_state.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        /* describe tesselation state */
        VkPipelineTessellationStateCreateInfo tesselation_state = {0};
        tesselation_state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

        /* describe viewport and scissor */
        VkPipelineViewportStateCreateInfo viewport_state = {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext         = Null,
            .flags         = 0,
            .viewportCount = 1,
            .pViewports    = (VkViewport[]
            ) {{.x        = 0,
                   .y        = 0,
                   .width    = surface->swapchain_image_extent.width,
                   .height   = surface->swapchain_image_extent.height,
                   .minDepth = 0.f,
                   .maxDepth = 1.f}},
            .scissorCount  = 1,
            .pScissors     = (VkRect2D[]
            ) {{.offset = {.x = 0, .y = 0}, .extent = surface->swapchain_image_extent}}
        };

        /* describe rasterization state */
        VkPipelineRasterizationStateCreateInfo rasterization_state = {
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext                   = Null,
            .flags                   = 0,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = VK_POLYGON_MODE_FILL,
            .cullMode                = VK_CULL_MODE_NONE,
            .frontFace               = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 1.f,
            .depthBiasClamp          = 0.f,
            .depthBiasSlopeFactor    = 1.f,
            .lineWidth               = 1.f
        };

        /* describe how multisampling of rendered images will be performed */
        VkPipelineMultisampleStateCreateInfo multisample_state = {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext                 = Null,
            .flags                 = 0,
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.f,
            .pSampleMask           = Null,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE
        };

        /* describe z-fighting behavior : disabled for now */
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {0};
        depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        /* we have a single color attachment in renderpass so we need this */
        VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
        color_blend_attachment.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable         = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

        /* describe color-blending */
        VkPipelineColorBlendStateCreateInfo color_blend_state = {0};
        color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state.attachmentCount = 1;
        color_blend_state.pAttachments    = &color_blend_attachment;

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext               = Null,
            .flags               = 0,
            .stageCount          = ARRAY_SIZE (shader_stages),
            .pStages             = shader_stages,
            .pVertexInputState   = &vertex_input_state,
            .pInputAssemblyState = &input_assembly_state,
            .pTessellationState  = &tesselation_state,
            .pViewportState      = &viewport_state,
            .pRasterizationState = &rasterization_state,
            .pMultisampleState   = &multisample_state,
            .pDepthStencilState  = &depth_stencil_state,
            .pColorBlendState    = &color_blend_state,
            .pDynamicState       = Null,
            .layout              = pipeline->pipeline_layout,
            .renderPass          = surface->render_pass,
            .subpass             = 0,
            .basePipelineHandle  = VK_NULL_HANDLE,
            .basePipelineIndex   = 0
        };

        /* create graphics pipelines */
        VkResult res = vkCreateGraphicsPipelines (
            device->device,
            VK_NULL_HANDLE,
            1,
            &graphics_pipeline_create_info,
            Null,
            &pipeline->pipeline
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            PIPELINE_CREATE_FAILED,
            "Failed to create graphics pipelines. RET = %d\n",
            res
        );

        /* destroy shader modules because we don't need them anymore */
        vkDestroyShaderModule (device->device, vert_shader, Null);
        vkDestroyShaderModule (device->device, frag_shader, Null);
    }

    return pipeline;

PIPELINE_CREATE_FAILED:
    vkDestroyPipelineLayout (device->device, pipeline->pipeline_layout, Null);
SHADER_LOAD_FAILED:
    if (vert_shader) {
        vkDestroyShaderModule (device->device, vert_shader, Null);
    }
    if (frag_shader) {
        vkDestroyShaderModule (device->device, frag_shader, Null);
    }
PIPELINE_LAYOUT_FAILED:
    FREE (pipeline);
    return Null;
}

/**
 * @b Destroy given ShaderPipeline object.
 *
 * @param pipeline 
 * @param device
 * */
void shader_pipeline_destroy (ShaderPipeline *pipeline, Device *device) {
    RETURN_IF (!pipeline || !device, ERR_INVALID_ARGUMENTS);

    vkDeviceWaitIdle (device->device);

    if (pipeline->pipeline_layout) {
        vkDestroyPipelineLayout (device->device, pipeline->pipeline_layout, Null);
        pipeline->pipeline_layout = VK_NULL_HANDLE;
    }

    if (pipeline->pipeline) {
        vkDestroyPipeline (device->device, pipeline->pipeline, Null);
        pipeline->pipeline = VK_NULL_HANDLE;
    }

    FREE (pipeline);
}

/**************************************************************************************************/
/******************************* PRIVATE HELPER METHOD DEFINITIONS ********************************/
/**************************************************************************************************/

/**
 * @b Create a shader module by loading it from file.
 *
 * @param device To use to create shader module.
 * @param path Path of SPIR-V shader code.
 *
 * @return VkShaderModule on success.
 * @return VK_NULL_HANDLE otherwise.
 * */
static inline VkShaderModule load_shader (VkDevice device, CString path) {
    RETURN_VALUE_IF (!device || !path, VK_NULL_HANDLE, ERR_INVALID_ARGUMENTS);

    FILE *file = fopen (path, "r");
    RETURN_VALUE_IF (!file, VK_NULL_HANDLE, ERR_INVALID_ARGUMENTS);

    fseek (file, 0, SEEK_END);
    Size file_size = ftell (file);
    GOTO_HANDLER_IF (!file_size, FILE_SIZE_ZERO, "Shader file (\"%s\") size must not be 0\n", path);
    fseek (file, 0, SEEK_SET);

    Uint8 *fdata = ALLOCATE (Uint8, file_size);
    fread (fdata, 1, file_size, file);
    fclose (file);

    VkShaderModuleCreateInfo shader_module_create_info = {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = Null,
        .flags    = 0,
        .codeSize = file_size,
        .pCode    = (Uint32 *)fdata
    };

    VkShaderModule shader = VK_NULL_HANDLE;
    VkResult       res = vkCreateShaderModule (device, &shader_module_create_info, Null, &shader);
    GOTO_HANDLER_IF (
        res != VK_SUCCESS,
        SHADER_CREATE_FAILED,
        "Failed to create shader module. RET = %d\n",
        res
    );

    FREE (fdata);

    return shader;

SHADER_CREATE_FAILED:
    FREE (fdata);
FILE_SIZE_ZERO:
    fclose (file);
    return VK_NULL_HANDLE;
}
