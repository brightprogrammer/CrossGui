/**
 * @file Device.h
 * @date Sun, 21st April 2024
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

#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H

#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* vulkan includes */
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

typedef struct DeviceQueue {
    Int32   family_index; /**< @b Non-negative value if holds a correct queue family index */
    VkQueue handle;       /**< @b Queue handle after creating device queue */
} DeviceQueue;

typedef struct Device {
    VkPhysicalDevice                 physical;
    VkDevice                         logical;
    VkPhysicalDeviceProperties       gpu_properties;
    VkPhysicalDeviceMemoryProperties gpu_mem_properties;
    DeviceQueue                      graphics_queue;
} Device;

Bool device_init();
Bool device_deinit();
Bool device_set_object_debug_name (VkObjectType object_type, Uint64 handle, CString name);

/**
 * @c Device buffer data.
 * */
typedef struct DeviceBuffer {
    Size                  size;   /**< @b Allocation size */
    VkBuffer              buffer; /**< @b Buffer handle */
    VkDeviceMemory        memory; /**< @b Buffer allocated memory handle. */
    VkBufferUsageFlags    usage;
    VkMemoryPropertyFlags mem_property;
    Uint32                queue_family_index;
    void* mapped_mem;
} DeviceBuffer;

DeviceBuffer *device_buffer_init (
    DeviceBuffer         *buffer,
    VkBufferUsageFlags    usage,
    Size                  size,
    VkMemoryPropertyFlags mem_property,
    Uint32                queue_family_inddex
);
DeviceBuffer *device_buffer_deinit (DeviceBuffer *buffer);
DeviceBuffer *device_buffer_memcpy (DeviceBuffer *buffer, void *data, Size size);
DeviceBuffer *device_buffer_resize (DeviceBuffer *buffer, Size size);

/**
 * @b Device image data.
 * */
typedef struct DeviceImage {
    VkImage           image;  /**< @b Image handle */
    VkDeviceMemory    memory; /**< @b Allocated device memory handle */
    VkImageView       view;   /**< @b Image view. */
    VkFormat          format; /**< @b Image format */
    VkExtent3D        extent; /**< @b Image dimensions. */
    VkImageUsageFlags usage;  /**< @b Image usage flags */
} DeviceImage;

DeviceImage *device_image_init (
    DeviceImage          *image,
    VkImageUsageFlags     usage,
    VkExtent3D            extent,
    VkFormat              format,
    VkMemoryPropertyFlags mem_property,
    VkImageAspectFlags    aspect_mask,
    Uint32                queue_family_inddex
);
DeviceImage *device_image_deinit (DeviceImage *image);
DeviceImage *device_image_change_layout (
    DeviceImage    *image,
    VkCommandBuffer cmd,
    VkImageLayout   initial_layout,
    VkImageLayout   final_layout
);
DeviceImage *device_image_clear (DeviceImage *image, VkCommandBuffer cmd, VkClearValue clear_value);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H
