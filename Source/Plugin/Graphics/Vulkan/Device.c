#include "Device.h"

/* libc includes */
#include <memory.h>

/**************************************************************************************************/
/******************************** DEVICE PUBLIC METHOD DEFINITIONS ********************************/
/**************************************************************************************************/

/**
 * @b Initialize device inside given @c Device object.
 *
 * @param device @c Device object to be initialized.
 *
 * @return @c device on success.
 * @return @c Null otherwise.
 * */
Bool device_init (Device *device, Vulkan *vk) {
    RETURN_VALUE_IF (!device || !vk, False, ERR_INVALID_ARGUMENTS);

    /* select gpu, get memory properties, get graphics queue family index */
    {
        /* selecting the first available gpu for now, will apply something complex probably never! */
        device->gpu = vk->gpus[0];

        /* get queue family indices */
        Int32        family_index = -1;
        VkQueueFlags queue_flags  = VK_QUEUE_GRAPHICS_BIT;
        {
            /* get queue family count */
            Uint32 queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties (device->gpu, &queue_family_count, Null);
            RETURN_VALUE_IF (
                !queue_family_count,
                False,
                "Failed to get queue family count in selected GPU\n"
            );

            /* get queue family properties */
            VkQueueFamilyProperties queue_family_properties[queue_family_count];
            vkGetPhysicalDeviceQueueFamilyProperties (
                device->gpu,
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

        device->graphics_queue.family_index = family_index;

        vkGetPhysicalDeviceMemoryProperties (device->gpu, &device->gpu_mem_properties);
    }

    /* create device */
    {
        /* create queue info for device */
        Float32                 queue_priorities  = 1.f;
        VkDeviceQueueCreateInfo queue_create_info = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = Null,
            .flags            = 0,
            .queueFamilyIndex = device->graphics_queue.family_index,
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
        VkResult res = VK_SUCCESS;
        res          = vkCreateDevice (device->gpu, &device_create_info, Null, &device->device);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            False,
            "Failed to create Logical Device. RES = %d\n",
            res
        );
    }

    /* fetch device queue handle */
    {
        vkGetDeviceQueue (
            device->device,
            device->graphics_queue.family_index,
            0,
            &device->graphics_queue.handle
        );
    }

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
Bool device_deinit (Device *device) {
    RETURN_VALUE_IF (!device, False, ERR_INVALID_ARGUMENTS);

    if (device->device) {
        vkDeviceWaitIdle (device->device);
        vkDestroyDevice (device->device, Null);
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
 * @param queue_family_index Index of queue family on which this buffer will be EXCLUSIVELY used.
 *
 * @return @c DeviceBuffer on success.
 * @return @c 0 otherwise.
 * */
DeviceBuffer *device_buffer_create (
    Device            *device,
    VkBufferUsageFlags usage,
    Size               size,
    Uint32             queue_family_index
) {
    RETURN_VALUE_IF (!device || !size, Null, ERR_INVALID_ARGUMENTS);

    /* create space for device buffer */
    DeviceBuffer *dbuf = NEW (DeviceBuffer);
    RETURN_VALUE_IF (!dbuf, Null, ERR_OUT_OF_MEMORY);

    /* create vertex buffer */
    VkBuffer vbuffer = VK_NULL_HANDLE;
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

        VkResult res = vkCreateBuffer (device->device, &buffer_create_info, Null, &vbuffer);
        GOTO_HANDLER_IF (res != VK_SUCCESS, BUFFER_FAILED, "Failed to create buffer object");
    }

    /* find required memory type */
    Size                  required_size     = 0;
    Uint32                memory_type_index = 0;
    VkMemoryPropertyFlags memory_property_flags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    {
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements (device->device, vbuffer, &memory_requirements);

        for (Uint32 i = 0; i < device->gpu_mem_properties.memoryTypeCount; i++) {
            /* if memory type bit is flagged, and property flags match then set that as memory type */
            if ((memory_requirements.memoryTypeBits & (1 << i)) &&
                ((device->gpu_mem_properties.memoryTypes[i].propertyFlags & memory_property_flags
                 ) == memory_property_flags)) {
                memory_type_index = i;
            }
        }

        GOTO_HANDLER_IF (
            !memory_type_index,
            MEMORY_TYPE_NOT_FOUND,
            "Required memory type not found!\n"
        );

        required_size = memory_requirements.size;
    }

    /* allocate memory for device buffer */
    VkDeviceMemory vmemory = VK_NULL_HANDLE;
    {
        VkMemoryAllocateInfo allocate_info = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = Null,
            .allocationSize  = required_size,
            .memoryTypeIndex = memory_type_index
        };

        VkResult res = vkAllocateMemory (device->device, &allocate_info, Null, &vmemory);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            MEMORY_ALLOC_FAILED,
            "Failed to allocate memory for new buffer\b"
        );
    }

    /* bind device buffer and device memory together to starting of memory (offset = 0) */
    Size offset = 0;
    vkBindBufferMemory (device->device, vbuffer, vmemory, offset);

    *dbuf = (DeviceBuffer) {.size = size, .buffer = vbuffer, .memory = vmemory};

    return dbuf;

MEMORY_TYPE_NOT_FOUND:
MEMORY_ALLOC_FAILED:
    vkDestroyBuffer (device->device, vbuffer, Null);
BUFFER_FAILED:
    FREE (dbuf);
    return Null;
}

/**
 * @b Destroy buffer object and free memory allocated on device.
 *
 * @param device 
 * @param device_buffer
 * */
void device_buffer_destroy (DeviceBuffer *dbuf, Device *device) {
    RETURN_IF (!device || !dbuf, ERR_INVALID_ARGUMENTS);

    vkDeviceWaitIdle (device->device);

    vkFreeMemory (device->device, dbuf->memory, Null);
    dbuf->memory = VK_NULL_HANDLE;

    vkDestroyBuffer (device->device, dbuf->buffer, Null);
    dbuf->buffer = VK_NULL_HANDLE;

    FREE (dbuf);
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
DeviceBuffer *device_buffer_memcpy (DeviceBuffer *dbuf, Device *device, void *data, Size size) {
    RETURN_VALUE_IF (!dbuf || !device || !data || !size, Null, ERR_INVALID_ARGUMENTS);

    void *mapped_data = Null;
    vkMapMemory (device->device, dbuf->memory, 0, size, 0, &mapped_data);
    memcpy (mapped_data, data, size);
    vkUnmapMemory (device->device, dbuf->memory);

    return dbuf;
}
