/**
 * @file RenderTarget.h
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

#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* local includes */
#include "Device.h"
#include "RenderTarget.h"
#include "Swapchain.h"
#include "Vulkan.h"

/* libc includes */
#include <memory.h>

/**
 * @b Initialize given @c RenderTargetSyncObjects.
 *
 * @param rtsync.
 *
 * @return @c rtsync on success.
 * @return @c Null otherwise.
 * */
RenderTargetSyncObjects *render_target_sync_objects_init (RenderTargetSyncObjects *rtsync) {
    RETURN_VALUE_IF (!rtsync, Null, ERR_INVALID_ARGUMENTS);

    VkDevice          device            = vk.device.logical;
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkResult res = vkCreateFence (device, &fence_create_info, Null, &rtsync->render_fence);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Fence. RET = %d\n", res);

    VkSemaphoreCreateInfo semaphore_create_info =
        {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = Null, .flags = 0};

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &rtsync->render_semaphore);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Semaphore. RET = %d\n", res);

    res = vkCreateSemaphore (device, &semaphore_create_info, Null, &rtsync->present_semaphore);
    GOTO_HANDLER_IF (res != VK_SUCCESS, INIT_FAILED, "Failed to create Semaphore. RET = %d\n", res);

    return rtsync;

INIT_FAILED:
    render_target_sync_objects_deinit (rtsync);
    return Null;
}

/**
 * @b De-initialize given @c RenderTargetSyncObjects.
 *
 * @param rtsync.
 *
 * @return @c rtsync on success.
 * @return @c Null otherwise.
 * */
RenderTargetSyncObjects *render_target_sync_objects_deinit (RenderTargetSyncObjects *rtsync) {
    RETURN_VALUE_IF (!rtsync, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    vkDeviceWaitIdle (device);

    if (rtsync->present_semaphore) {
        vkDestroySemaphore (device, rtsync->present_semaphore, Null);
    }
    if (rtsync->render_semaphore) {
        vkDestroySemaphore (device, rtsync->render_semaphore, Null);
    }
    if (rtsync->render_fence) {
        vkDestroyFence (device, rtsync->render_fence, Null);
    }

    /* restore to invalid state */
    memset (rtsync, 0, sizeof (RenderTarget));

    return rtsync;
}
