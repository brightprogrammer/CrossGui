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

RenderTarget *render_target_init (
    RenderTarget   *rt,
    VkCommandPool   cmd_pool,
    SwapchainImage *color_image,
    DeviceImage    *depth_image
) {
    RETURN_VALUE_IF (!rt || !cmd_pool || !depth_image || !color_image, Null, ERR_INVALID_ARGUMENTS);

    VkDevice device = vk.device.logical;

    /* allocate command buffers */
    {
        /* NOTE: Allocating single command buffers at a time can be slow,
         * but this is a rare operation, so I gues this won't matter that much. */
        VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = Null,
            .commandPool        = cmd_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        /* allocate command buffer(s) */
        VkResult res =
            vkAllocateCommandBuffers (device, &command_buffer_allocate_info, &rt->cmd_buffer);

        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            Null,
            "Failed to allocate Command Buffers. RET = %d\n",
            res
        );
    }

    /* create framebuffers */

    /* create fences and semaphores */

    return rt;
}

RenderTarget *render_target_deinit (RenderTarget *rt);
