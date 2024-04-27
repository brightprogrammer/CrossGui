/**
 * @file ShaderResourceBinding.h
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

#ifndef ANVIE_CROSSGUI_SOURCE_PLUGINS_GRAPHICS_VULKAN_SHADER_RESOURCE_BINDING_H
#define ANVIE_CROSSGUI_SOURCE_PLUGINS_GRAPHICS_VULKAN_SHADER_RESOURCE_BINDING_H

#include <Anvie/Types.h>

/* vulkan includes */
#include <vulkan/vulkan.h>

typedef struct DeviceBuffer DeviceBuffer; /* TODO: Remove this from SRB Object. */
typedef struct Swapchain    Swapchain;

// NOTE : Create a single unifotm data to store UI mesh data.
// This will include things like mesh data of button, rounded or plain, or circular, etc...
// This uniform is a global uniform and will always be bound. At top of the hierarchy of
// shader resource bindings.
// Use GpuUiData for passing all these data
typedef struct GpuUiData {
    Float32 button_width;  /**< @b Scale width of all buttons by this factor */
    Float32 button_height; /**< @b Scale height of all buttons by this factor */
} GpuUiData;

typedef struct ShaderResourceBinding {
    VkDescriptorPool      descriptor_pool;
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_set;

    DeviceBuffer *uniform_buffer; /**< @b Uniform buffer to be sent to shader */
} ShaderResourceBinding;

ShaderResourceBinding *shader_resource_binding_create_ui_binding();
void                   shader_resource_binding_destroy (ShaderResourceBinding *srb);

typedef struct ShaderPipeline {
    VkPipelineLayout pipeline_layout;
    VkPipeline       pipeline;
} ShaderPipeline;

ShaderPipeline  *
    shader_pipeline_create_ui_pipeline (Swapchain *swapchain, ShaderResourceBinding *srb);
void shader_pipeline_destroy (ShaderPipeline *pipeline);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGINS_GRAPHICS_VULKAN_SHADER_RESOURCE_BINDING_H
