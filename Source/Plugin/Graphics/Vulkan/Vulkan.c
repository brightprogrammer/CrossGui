/**
 * @file Vulkan.c 
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

/* libc */
#include <memory.h>

/* crosswindow */
#include <Anvie/CrossWindow/Vulkan.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Graphics/Graphics.h>
#include <Anvie/CrossGui/Plugin/Plugin.h>

/* local includes */
#include "Device.h"
#include "GraphicsContext.h"
#include "Renderer.h"
#include "Swapchain.h"
#include "Vulkan.h"

Vulkan vk = {0};

static Bool init();
static Bool deinit();
static Bool
    draw_2d (XuiGraphicsContext *gctx, XwWindow *xwin, Vertex2D *vertices, Size vertex_count);

static Bool init() {
    /* create vulkan instance */
    {
        /* get names of required layers and their count as well */
        Size           layer_count = 0;
        const CString *layers      = Null;
        {
            static const CString required_layers[] = {"VK_LAYER_KHRONOS_validation"};
            layer_count                            = ARRAY_SIZE (required_layers);
            layers                                 = required_layers;
        }

        /* get names of required extension names and their count as well */
        Size           extension_count = 0;
        const CString *extensions      = Null;
        {
            /* Extensions required by CrossWindow */
            Size     window_ext_count = 0;
            CString *window_exts      = xw_get_required_extension_names (&window_ext_count);

            /* Extensions required by this application on it's own 
             * Any new required extensions must be added to this array */
            static const CString my_exts[]    = {};
            Size                 my_ext_count = ARRAY_SIZE (my_exts);

            /* total number of extensions */
            Size ext_count = window_ext_count + my_ext_count;

            /* extensions required by all */
            static CString exts[8];
            for (Size s = 0; s < window_ext_count; s++) {
                exts[s] = window_exts[s];
            }
            for (Size s = 0; s < my_ext_count; s++) {
                exts[window_ext_count + s] = my_exts[s];
            }

            /* store extension count and extension names */
            extension_count = ext_count;
            extensions      = exts;
        }

        /* set create info structure */
        VkInstanceCreateInfo instance_create_info = {
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = Null,
            .flags                   = 0,
            .pApplicationInfo        = Null,
            .enabledLayerCount       = layer_count,
            .ppEnabledLayerNames     = layers,
            .enabledExtensionCount   = extension_count,
            .ppEnabledExtensionNames = extensions
        };

        /* create vulkan instance */
        VkResult res = vkCreateInstance (&instance_create_info, Null, &vk.instance);
        RETURN_VALUE_IF (
            res != VK_SUCCESS,
            False,
            "Failed to create Vulkan instance. RES = %d\n",
            res
        );
    }

    /* get all GPU handles */
    {
        /* get the number of gpus on host */
        VkResult res = vkEnumeratePhysicalDevices (vk.instance, &vk.gpu_count, Null);
        GOTO_HANDLER_IF (
            res != VK_SUCCESS,
            INIT_FAILED,
            "Failed to get GPU count. RET = %d\n",
            res
        );

        /* get the gpu handles */
        vk.gpus = Null;
        if (vk.gpu_count) {
            vk.gpus = ALLOCATE (VkPhysicalDevice, vk.gpu_count);
            GOTO_HANDLER_IF (!vk.gpus, INIT_FAILED, ERR_OUT_OF_MEMORY);

            /* get gpu handles */
            res = vkEnumeratePhysicalDevices (vk.instance, &vk.gpu_count, vk.gpus);
            GOTO_HANDLER_IF (
                res != VK_SUCCESS,
                INIT_FAILED,
                "Failed to get GPU handles. RET = %d\n",
                res
            );
        }
    }

    /* initialize commonly shared device */
    GOTO_HANDLER_IF (!device_init(), INIT_FAILED, "Failed to initialize logical device.\n");

    return True;

INIT_FAILED:
    deinit();
    return False;
}

static Bool deinit() {
    /* deinit logical device if created */
    if (vk.device.logical) {
        device_deinit();
    }

    /* free allocated gpu array */
    if (vk.gpus) {
        FREE (vk.gpus);
        vk.gpus = Null;
    }

    /* destroy instance if created */
    if (vk.instance) {
        vkDestroyInstance (vk.instance, Null);
    }

    /* remove references to any handles and pointers */
    memset (&vk, 0, sizeof (Vulkan));

    return True;
}

/* Describe callbacks in graphics plugin data */
static XuiGraphicsPlugin vulkan_graphics_plugin_data = {
    /* graphics context related methods */
    .context_create  = graphics_context_create,
    .context_destroy = graphics_context_destroy,
    .context_resize  = graphics_context_resize,

    /* drawing methods */
    .draw_rect_2d = draw_rect_2d
};

/**
 * @b Vulkan Graphics Plugin
 * */
XuiPlugin xui_plugin = {
    .type                = XUI_PLUGIN_TYPE_GRAPHICS,
    .name                = "Vulkan Graphics Plugin",
    .version             = {.date = 20, .month = 4, .year = 2024},
    .license             = "BSD 3-Clause License",
    .supported_platforms = XUI_PLUGIN_PLATFORM_MASK_LINUX,
    .plugin_data         = &vulkan_graphics_plugin_data,
    .init                = (XuiPluginInit)init,
    .deinit              = (XuiPluginDeinit)deinit
};
