#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_VULKAN_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_VULKAN_H

#include <Anvie/Types.h>

/* vulkan includes */
#include <vulkan/vulkan.h>

typedef struct Vulkan {
    VkInstance        instance;  /**< @b Our connection with vulkan */
    Uint32            gpu_count; /**< @b Total number of usable physical devices on host. */
    VkPhysicalDevice *gpus;      /**< @b GPU handles */
} Vulkan;

#endif                           // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_VULKAN_H
