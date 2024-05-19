/**
 * @file GraphicsPipeline.c
 * @date Sat, 27th April 2024
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

/* crossgui plugin */
#include <Anvie/CrossGui/Utils/Maths.h>
#include <vulkan/vulkan_core.h>

/* local includes */
#include "Anvie/Common.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "Swapchain.h"
#include "Vulkan.h"

/**************************************************************************************************/
/********************************** PRIVATE METHOD DELCARATIONS ***********************************/
/**************************************************************************************************/

/* private helper methods */
static inline VkShaderModule load_shader (VkDevice device, CString path);

/**************************************************************************************************/
/*********************** SHADER RESOURCE BINDING PUBLIC METHOD DEFINITIONS ************************/
/**************************************************************************************************/

/**
 * @b Create a new Shader UI binding.
 *
 * @return @c ShaderResourceBinding on success.
 * @return @c Null otherwise.
 * */
GraphicsPipeline *graphics_pipeline_init_default (
    GraphicsPipeline *pipeline,
    RenderPass       *render_pass,
    Swapchain        *swapchain
) {
    RETURN_VALUE_IF (!pipeline || !render_pass || !swapchain, Null, ERR_OUT_OF_MEMORY);

    VkDevice       device      = vk.device.logical;
    VkShaderModule vert_shader = VK_NULL_HANDLE, frag_shader = VK_NULL_HANDLE;

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
            device,
            &descriptor_pool_create_info,
            Null,
            &pipeline->descriptor_pool
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
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
            device,
            &set_layout_create_info,
            Null,
            &pipeline->descriptor_set_layout
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create descriptor set layout. RET = %d\n",
            res
        );
    }

    /* allocate one single descriptor set */
    {
        VkDescriptorSetAllocateInfo set_allocate_info = {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = Null,
            .descriptorPool     = pipeline->descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts        = (VkDescriptorSetLayout[]) {pipeline->descriptor_set_layout}
        };

        VkResult res =
            vkAllocateDescriptorSets (device, &set_allocate_info, &pipeline->descriptor_set);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create descriptor set layout. RET = %d\n",
            res
        );
    }

    /* create a pipeline layout including provided shader resource binding */
    {
        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext                  = Null,
            .flags                  = 0,
            .setLayoutCount         = 1,
            .pSetLayouts            = (VkDescriptorSetLayout[]) {pipeline->descriptor_set_layout},
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = Null
        };

        VkResult res = vkCreatePipelineLayout (
            device,
            &pipeline_layout_create_info,
            Null,
            &pipeline->pipeline_layout
        );

        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create pipeline layout. RET = %d\n",
            res
        );
    }

    /* create pipeline */
    {
        vert_shader = load_shader (device, "bin/Shaders/triangle.vert.spv");
        frag_shader = load_shader (device, "bin/Shaders/triangle.frag.spv");
        GOTO_HANDLER_IF (
            !vert_shader || !frag_shader,
            INIT_FAILED,
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

        /*
         * NOTE : Instead of sending vertex positions and vertex colors together, we
         * send in position data only and send color and scale separately as a uniform.
         * This allows reuse of position data and different color support.
         *
         * Therfore, we just describe position as input attribute below
         * */

        /* describe how vertex data is sent to GPU */
        VkVertexInputBindingDescription vertex_binding_desc =
            {.binding = 0, .stride = sizeof (Vec2f), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

        VkVertexInputAttributeDescription vertex_attribute_desc[] = {
            {.location = 0, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = 0}
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
                   .width    = swapchain->image_extent.width,
                   .height   = swapchain->image_extent.height,
                   .minDepth = 0.f,
                   .maxDepth = 1.f}},
            .scissorCount  = 1,
            .pScissors =
                (VkRect2D[]) {{.offset = {.x = 0, .y = 0}, .extent = swapchain->image_extent}}
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

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext                 = Null,
            .flags                 = 0,
            .depthTestEnable       = VK_TRUE,
            .depthWriteEnable      = VK_TRUE,
            .depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .front                 = (VkStencilOpState) {0},
            .back                  = (VkStencilOpState) {0},
            .minDepthBounds        = 0,
            .maxDepthBounds        = 1,
        };
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
            .renderPass          = render_pass->render_pass,
            .subpass             = 0,
            .basePipelineHandle  = VK_NULL_HANDLE,
            .basePipelineIndex   = 0
        };

        /* create graphics pipelines */
        VkResult res = vkCreateGraphicsPipelines (
            device,
            VK_NULL_HANDLE,
            1,
            &graphics_pipeline_create_info,
            Null,
            &pipeline->pipeline
        );
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create graphics pipelines. RET = %d\n",
            res
        );

        /* destroy shader modules because we don't need them anymore */
        vkDestroyShaderModule (device, vert_shader, Null);
        vkDestroyShaderModule (device, frag_shader, Null);
    }

    return pipeline;

INIT_FAILED:

    if (vert_shader) {
        vkDestroyShaderModule (device, vert_shader, Null);
    }
    if (frag_shader) {
        vkDestroyShaderModule (device, frag_shader, Null);
    }

    graphics_pipeline_deinit (pipeline);

    return Null;
}

/**
 * @b Destroy given ShaderResourceBinding object.
 *
 * @param srb 
 * @param device
 * */
GraphicsPipeline *graphics_pipeline_deinit (GraphicsPipeline *pipeline) {
    RETURN_VALUE_IF (!pipeline, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    if (pipeline->pipeline) {
        vkDestroyPipeline (device, pipeline->pipeline, Null);
        pipeline->pipeline = VK_NULL_HANDLE;
    }

    if (pipeline->pipeline_layout) {
        vkDestroyPipelineLayout (device, pipeline->pipeline_layout, Null);
        pipeline->pipeline_layout = VK_NULL_HANDLE;
    }

    if (pipeline->descriptor_pool) {
        vkDestroyDescriptorPool (device, pipeline->descriptor_pool, Null);
        pipeline->descriptor_pool = VK_NULL_HANDLE;
        pipeline->descriptor_set  = VK_NULL_HANDLE;
    }

    if (pipeline->descriptor_set_layout) {
        vkDestroyDescriptorSetLayout (device, pipeline->descriptor_set_layout, Null);
        pipeline->descriptor_set_layout = VK_NULL_HANDLE;
    }

    return pipeline;
}

/**
 * @b Write given uniform buffer's information to default descriptor set.
 *
 * @param pipeline
 * @param uniform_barrier
 *
 * @return @p pipeline on success.
 * @return @c Null otherwise.
 * */
GraphicsPipeline *graphics_pipeline_write_to_descriptor_set (
    GraphicsPipeline *pipeline,
    DeviceBuffer     *uniform_buffer
) {
    RETURN_VALUE_IF (!pipeline || !uniform_buffer, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* write this buffer info to descriptor set */
    {
        VkDescriptorBufferInfo buffer_info =
            {.buffer = uniform_buffer->buffer, .range = uniform_buffer->size, .offset = 0};

        VkWriteDescriptorSet write_descriptor_set = {
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = Null,
            .dstSet           = pipeline->descriptor_set,
            .dstBinding       = 0,
            .dstArrayElement  = 0,
            .descriptorCount  = 1,
            .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo       = Null,
            .pBufferInfo      = &buffer_info,
            .pTexelBufferView = Null
        };

        vkUpdateDescriptorSets (device, 1, &write_descriptor_set, 0, Null);
    }

    return pipeline;
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
    RETURN_VALUE_IF (!file, VK_NULL_HANDLE, ERR_FILE_OPEN_FAILED);

    fseek (file, 0, SEEK_END);
    Size file_size = ftell (file);
    GOTO_HANDLER_IF (!file_size, FILE_SIZE_ZERO, ERR_FILE_READ_FAILED);
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
