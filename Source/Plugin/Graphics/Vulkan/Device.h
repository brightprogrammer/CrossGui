#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H

#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* vulkan includes */
#include <vulkan/vulkan.h>

/* local includes */
#include "Vulkan.h"

typedef struct DeviceQueue {
    Int32   family_index; /**< @b Non-negative value if holds a correct queue family index */
    VkQueue handle;       /**< @b Queue handle after creating device queue */
} DeviceQueue;

typedef struct Device {
    VkPhysicalDevice                 gpu;
    VkDevice                         device;
    VkPhysicalDeviceProperties       gpu_properties;
    VkPhysicalDeviceMemoryProperties gpu_mem_properties;
    DeviceQueue                      graphics_queue;
} Device;

Bool device_init (Device *device, Vulkan *vk);
Bool device_deinit (Device *device);

/**
 * @c Device buffer data.
 * */
typedef struct DeviceBuffer {
    Size           size;
    VkBuffer       buffer;
    VkDeviceMemory memory;
} DeviceBuffer;

DeviceBuffer *device_buffer_create (
    Device            *device,
    VkBufferUsageFlags usage,
    Size               size,
    Uint32             queue_family_inddex
);
void          device_buffer_destroy (DeviceBuffer *dbuf, Device *device);
DeviceBuffer *device_buffer_memcpy (DeviceBuffer *dbuf, Device *device, void *data, Size size);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_H
