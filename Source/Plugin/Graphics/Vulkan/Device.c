/**
 * @file Device.c
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

#include <Anvie/Common.h>

/* local includes */
#include "Device.h"
#include "Vulkan.h"

/* libc includes */
#include <memory.h>
#include <vulkan/vulkan_core.h>

/**************************************************************************************************/
/******************************** DEVICE PUBLIC METHOD DEFINITIONS ********************************/
/**************************************************************************************************/

/**
 * @b Initialize device inside global Vulkan state @c Device object.
 *
 * @param device @c Device object to be initialized.
 *
 * @return @c device on success.
 * @return @c Null otherwise.
 * */
Bool device_init() {
    /* selecting the first available gpu for now, will apply something complex probably never! */
    vk.device.physical = vk.gpus[0];

    /* get selected gpu handle */
    VkPhysicalDevice gpu = vk.device.physical;

    /* select gpu, get memory properties, get graphics queue family index */
    {
        /* get queue family indices */
        Int32        family_index = -1;
        VkQueueFlags queue_flags  = VK_QUEUE_GRAPHICS_BIT;
        {
            /* get queue family count */
            Uint32 queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties (gpu, &queue_family_count, Null);
            RETURN_VALUE_IF (
                !queue_family_count,
                False,
                "Failed to get queue family count in selected GPU\n"
            );

            /* get queue family properties */
            VkQueueFamilyProperties queue_family_properties[queue_family_count];
            vkGetPhysicalDeviceQueueFamilyProperties (
                gpu,
                &queue_family_count,
                queue_family_properties
            );

            /* find queue family with given queue flags */
            for (Size s = 0; s < queue_family_count; s++) {
                if ((queue_family_properties[s].queueFlags & queue_flags) == queue_flags) {
                    family_index = s;
                }
            }
        }

        /* make sure we got the queue family index */
        RETURN_VALUE_IF (
            family_index == -1,
            False,
            "Failed to find queue family with queue flags \"%x\" in selected GPU\n",
            queue_flags
        );

        vk.device.graphics_queue.family_index = family_index;

        vkGetPhysicalDeviceMemoryProperties (gpu, &vk.device.gpu_mem_properties);
    }

    /* create device */
    {
        /* create queue info for device */
        Float32                 queue_priorities  = 1.f;
        VkDeviceQueueCreateInfo queue_create_info = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = Null,
            .flags            = 0,
            .queueFamilyIndex = vk.device.graphics_queue.family_index,
            .queueCount       = 1,
            .pQueuePriorities = &queue_priorities
        };

        /* fill in required extensions here */
        static const CString extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo device_create_info = {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = Null,
            .flags                   = 0,
            .queueCreateInfoCount    = 1,
            .pQueueCreateInfos       = &queue_create_info,
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = Null,
            .enabledExtensionCount   = ARRAY_SIZE (extensions),
            .ppEnabledExtensionNames = extensions,
            .pEnabledFeatures        = Null
        };

        /* create device */
        VkResult res = vkCreateDevice (gpu, &device_create_info, Null, &vk.device.logical);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            False,
            "Failed to create Logical Device. RES = %d\n",
            res
        );
    }

    /* fetch device queue handle */
    vkGetDeviceQueue (
        vk.device.logical,
        vk.device.graphics_queue.family_index,
        0,
        &vk.device.graphics_queue.handle
    );

    return True;
}

/**
 * @b De-initialize already initialized @c Device object.
 *
 * @param device 
 *
 * @return @c device on success.
 * @return @c Null otherwise.
 * */
Bool device_deinit() {
    VkDevice device = vk.device.logical;

    if (device) {
        vkDeviceWaitIdle (device);
        vkDestroyDevice (device, Null);
    }

    memset (device, 0, sizeof (Device));

    return True;
}

/**************************************************************************************************/
/********************************** DEVICE BUFFER PUBLIC METHODS **********************************/
/**************************************************************************************************/

/**
 * @b Create a buffer object.
 *
 * @param device Device to allocate memory on.
 * @param usage How will this buffer object be used.
 * @param size Number of bytes to allocate.
 * @param mem_property Bitmask of @c VkMemoryPropertyFlagBits representing designed memory properties.
 * @param queue_family_index Index of queue family on which this buffer will be EXCLUSIVELY used.
 *
 * @return @c DeviceBuffer on success.
 * @return @c 0 otherwise.
 * */
DeviceBuffer *device_buffer_init (
    DeviceBuffer         *buffer,
    VkBufferUsageFlags    usage,
    Size                  size,
    VkMemoryPropertyFlags mem_property,
    Uint32                queue_family_index
) {
    RETURN_VALUE_IF (!buffer || !size, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* create buffer */
    {
        VkBufferCreateInfo buffer_create_info = {
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext                 = Null,
            .flags                 = 0,
            .size                  = size,
            .usage                 = usage,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices   = (Uint32[]) {queue_family_index}
        };

        VkResult res = vkCreateBuffer (device, &buffer_create_info, Null, &buffer->buffer);
        RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create buffer object");
    }


    /* allocate memory for device buffer */
    {
        /* find required memory type index and allocation size */
        Size   required_size     = 0;
        Uint32 memory_type_index = 0;
        {
            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements (device, buffer->buffer, &memory_requirements);

            for (Uint32 i = 0; i < vk.device.gpu_mem_properties.memoryTypeCount; i++) {
                /* if memory type bit is flagged, and property flags match then set that as memory type */
                if ((memory_requirements.memoryTypeBits & (1 << i)) &&
                    ((vk.device.gpu_mem_properties.memoryTypes[i].propertyFlags & mem_property) ==
                     mem_property)) {
                    memory_type_index = i;
                }
            }

            GOTO_HANDLER_IF (!memory_type_index, INIT_FAILED, "Required memory type not found!\n");

            required_size = memory_requirements.size;
        }

        VkMemoryAllocateInfo allocate_info = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = Null,
            .allocationSize  = required_size,
            .memoryTypeIndex = memory_type_index
        };

        VkResult res = vkAllocateMemory (device, &allocate_info, Null, &buffer->memory);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to allocate memory for new buffer\b"
        );
    }

    /* bind device buffer and device memory together to starting of memory (offset = 0) */
    Size offset = 0;
    vkBindBufferMemory (device, buffer->buffer, buffer->memory, offset);

    return buffer;

INIT_FAILED:
    device_buffer_deinit (buffer);
    return Null;
}

/**
 * @b De-initialize @c DeviceBuffer object and free memory allocated on device.
 *
 * @param buffer
 *
 * @return @c buffer on succes.
 * @return @c Null otherwise.
 * */
DeviceBuffer *device_buffer_deinit (DeviceBuffer *buffer) {
    RETURN_VALUE_IF (!buffer, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    if (buffer->memory) {
        vkFreeMemory (device, buffer->memory, Null);
    }

    if (buffer->buffer) {
        vkDestroyBuffer (device, buffer->buffer, Null);
    }

    memset (buffer, 0, sizeof (DeviceBuffer));

    return buffer;
}

/**
 * @b Upload buffer data to GPU by copying it from given main memory to device memory.
 *
 * @param bo 
 * @param device 
 * @param data Pointer to data to be copied.
 * @param size Size of data to be copied in number of bytes.
 *
 * @return @c bo on success.
 * @return @c {0, 0} otherwise
 * */
DeviceBuffer *device_buffer_memcpy (DeviceBuffer *buffer, void *data, Size size) {
    RETURN_VALUE_IF (!buffer || !data || !size, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    void *mapped_data = Null;
    vkMapMemory (device, buffer->memory, 0, size, 0, &mapped_data);
    memcpy (mapped_data, data, size);
    vkUnmapMemory (device, buffer->memory);

    return buffer;
}

/**
 * @b Initialize the given @c DeviceImage object.
 *
 * @param image Pointer to memory where @c DeviceImage object must be initialized.
 * @param usage Image usage flags.
 * @param width Image width.
 * @param height Image height.
 * @param format Image format (depending on image usage).
 * @param mem_property Bitmask of @c VkMemoryPropertyFlagBits representing designed memory properties.
 * @param queue_family_inddex Which queue family this image belongs to?
 *
 * @return @c image on success.
 * @return @c Null otherwise.
 * */
DeviceImage *device_image_init (
    DeviceImage          *image,
    VkImageUsageFlags     usage,
    Uint32                width,
    Uint32                height,
    VkFormat              format,
    VkImageAspectFlags    aspect_mask,
    VkMemoryPropertyFlags mem_property,
    Uint32                queue_family_inddex
) {
    RETURN_VALUE_IF (
        !image || !width || !height || queue_family_inddex == (Uint32)-1,
        Null,
        ERR_INVALID_ARGUMENTS
    );

    VkDevice device = vk.device.logical;

    /* create image */
    {
        VkImageCreateInfo image_create_info = {
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = Null,
            .flags                 = 0,
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = format,
            .extent                = (VkExtent3D) {width, height, 1},
            .mipLevels             = 1,
            .arrayLayers           = 1,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = usage,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices   = ((Uint32[]) {vk.device.graphics_queue.family_index}
                              ),
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VkResult res = vkCreateImage (device, &image_create_info, Null, &image->image);
        RETURN_VALUE_IF (res != VK_SUCCESS, Null, "Failed to create device image. RET = %d\n", res);
    }

    /* find memory requirements and allocate memory for image */
    {
        /* find required memory type index and allocation size */
        Size   required_size     = 0;
        Uint32 memory_type_index = 0;
        {
            VkMemoryRequirements memory_requirements;
            vkGetImageMemoryRequirements (device, image->image, &memory_requirements);

            for (Uint32 i = 0; i < vk.device.gpu_mem_properties.memoryTypeCount; i++) {
                /* if memory type bit is flagged, and property flags match then set that as memory type */
                if ((memory_requirements.memoryTypeBits & (1 << i)) &&
                    ((vk.device.gpu_mem_properties.memoryTypes[i].propertyFlags & mem_property) ==
                     mem_property)) {
                    memory_type_index = i;
                }
            }

            GOTO_HANDLER_IF (!memory_type_index, INIT_FAILED, "Required memory type not found!\n");

            required_size = memory_requirements.size;
        }

        VkMemoryAllocateInfo allocate_info = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = Null,
            .allocationSize  = required_size,
            .memoryTypeIndex = memory_type_index
        };

        VkResult res = vkAllocateMemory (device, &allocate_info, Null, &image->memory);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to allocate memory for new buffer\b"
        );
    }

    /* create image view */
    {
        VkImageViewCreateInfo image_view_create_info = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = Null,
            .flags    = 0,
            .image    = image->image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = format,
            .components =
                {
                             .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                             },
            .subresourceRange =
                {.aspectMask = aspect_mask, .baseArrayLayer = 0, .levelCount = 1, .baseMipLevel = 0
                }
        };

        VkResult res = vkCreateImageView (device, &image_view_create_info, Null, &image->view);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to create iamge view for device image. RET = %d\n",
            res
        );
    }

    return image;

INIT_FAILED:
    device_image_deinit (image);
    return Null;
}

/**
 * @b De-initialize given @c DeviceImage object and free memory allocated on device.
 *
 * @param image
 *
 * @return @c image on success.
 * @return @c Null otherwise.
 * */
DeviceImage *device_image_deinit (DeviceImage *image) {
    RETURN_VALUE_IF (!image, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    if (image->view) {
        vkDestroyImageView (device, image->view, Null);
    }

    if (image->memory) {
        vkFreeMemory (device, image->memory, Null);
    }

    if (image->image) {
        vkDestroyImage (device, image->image, Null);
    }

    memset (image, 0, sizeof (DeviceImage));

    return image;
}
