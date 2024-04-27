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

Bool device_init (Device *device);
Bool device_deinit (Device *device);

/**
 * @c Device buffer data.
 * */
typedef struct DeviceBuffer {
    Size           size;   /**< @b Allocation size */
    VkBuffer       buffer; /**< @b Buffer handle */
    VkDeviceMemory memory; /**< @b Buffer allocated memory handle. */
} DeviceBuffer;

DeviceBuffer *device_buffer_create (
    Device            *device,
    VkBufferUsageFlags usage,
    Size               size,
    Uint32             queue_family_inddex
);
void          device_buffer_destroy (DeviceBuffer *dbuf, Device *device);
DeviceBuffer *device_buffer_memcpy (DeviceBuffer *dbuf, Device *device, void *data, Size size);

/**
 * @b Device image data.
 * */
typedef struct DeviceImage {
    Uint32         width;  /**< @b Image width. */
    Uint32         height; /**< @b Image height. */
    VkImage        image;  /**< @b Image handle */
    VkDeviceMemory memory; /**< @b Allocated device memory handle */
    VkImageView    view;   /**< @b Image view. */
    VkFormat       format; /**< @b Image format */
} DeviceImage;

DeviceImage *device_image_create (
    Device           *device,
    VkImageUsageFlags usage,
    Uint32            width,
    Uint32            height,
    VkFormat          format,
    Uint32            queue_family_inddex
);
void device_image_destroy (DeviceImage *dimg, Device *device);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H
