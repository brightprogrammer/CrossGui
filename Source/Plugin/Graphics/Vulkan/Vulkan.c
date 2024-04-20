/* libc */
#include <memory.h>

/* crosswindow */
#include <Anvie/CrossWindow/Vulkan.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Graphics/Graphics.h>
#include <Anvie/CrossGui/Plugin/Plugin.h>

/* local includes */
#include "Anvie/Common.h"
#include "Device.h"
#include "Surface.h"
#include "Vulkan.h"

static Vulkan vk = {0};

typedef struct XuiGraphicsContext {
    Device  device;
    Surface surface;
} XuiGraphicsContext;

static Bool                init();
static Bool                deinit();
static XuiGraphicsContext *graphics_context_create (XwWindow *xwin);
static void                graphics_context_destroy (XuiGraphicsContext *gctx);
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

    return True;

INIT_FAILED:
    deinit();
    return False;
}

static Bool deinit() {
    if (vk.gpus) {
        FREE (vk.gpus);
        vk.gpus = Null;
    }

    if (vk.instance) {
        vkDestroyInstance (vk.instance, Null);
    }

    memset (&vk, 0, sizeof (Vulkan));

    return True;
}

static XuiGraphicsContext *graphics_context_create (XwWindow *xwin) {
    RETURN_VALUE_IF (!xwin, Null, ERR_INVALID_ARGUMENTS);

    XuiGraphicsContext *gctx = NEW (XuiGraphicsContext);
    RETURN_VALUE_IF (!gctx, Null, ERR_OUT_OF_MEMORY);

    GOTO_HANDLER_IF (
        !device_init (&gctx->device, &vk) ||
            !surface_init (&gctx->surface, &gctx->device, &vk, xwin),
        GCTX_FAILED,
        "Failed to create device context\n"
    );

    return gctx;

GCTX_FAILED:
    graphics_context_destroy (gctx);
    return Null;
}

static void graphics_context_destroy (XuiGraphicsContext *gctx) {
    RETURN_IF (!gctx, ERR_INVALID_ARGUMENTS);

    surface_deinit (&gctx->surface, &gctx->device, &vk);
    device_deinit (&gctx->device);

    FREE (gctx);
}

static Bool
    draw_2d (XuiGraphicsContext *gctx, XwWindow *xwin, Vertex2D *vertices, Size vertex_count) {
    RETURN_VALUE_IF (!gctx || !xwin || !vertices || !vertex_count, False, ERR_INVALID_ARGUMENTS);

    VkFence fences[] = {gctx->surface.render_fence};

    /* wait for all gpu rendering to complete */
    VkResult res = vkWaitForFences (gctx->device.device, ARRAY_SIZE (fences), fences, True, 1e9);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Timeout (1s) while waiting for fences. RET = %d\n",
        res
    );

    /* get next image index */
    Uint32 next_image_index = -1;

    res = vkAcquireNextImageKHR (
        gctx->device.device,
        gctx->surface.swapchain,
        1e9,
        gctx->surface.present_semaphore,
        Null,
        &next_image_index
    );

    /* TODO: handle window resize here! */
    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
        RETURN_VALUE_IF (
            !surface_recreate_swapchain (&gctx->surface, &gctx->device, xwin),
            False,
            "Failed to recreate swapchain\n"
        );
        return True;
        res = VK_SUCCESS; /* to pass upcoming check */
    }
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to get next image index from swapchain. RET = %d\n",
        res
    );

    /* need to reset fence before we use it again */
    res = vkResetFences (gctx->device.device, ARRAY_SIZE (fences), fences);
    RETURN_VALUE_IF (res != VK_SUCCESS, False, "Failed to reset fences. RET = %d\n", res);

    /* reset command buffer and record draw commands again */
    res = vkResetCommandBuffer (gctx->surface.cmd_buffer, 0);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to reset command buffer for recording new commands. RET = %d\n",
        res
    );

    VkCommandBuffer          cmd            = gctx->surface.cmd_buffer;
    VkCommandBufferBeginInfo cmd_begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = Null,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = Null
    };

    res = vkBeginCommandBuffer (cmd, &cmd_begin_info);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to begin command buffer recording. RET = %d\n",
        res
    );

    VkClearValue clear_value = {.color = {{1, 0.6, 0.8, 1}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = Null,
        .renderPass  = gctx->surface.render_pass,
        .renderArea  = {.offset = {.x = 0, .y = 0}, .extent = gctx->surface.swapchain_image_extent},
        .framebuffer = gctx->surface.framebuffers[next_image_index],
        .clearValueCount = 1,
        .pClearValues    = &clear_value
    };

    vkCmdBeginRenderPass (cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass (cmd);

    res = vkEndCommandBuffer (cmd);
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to end command buffer recording. RET = %d\n",
        res
    );

    /* wait when rendered image is being presented */
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = Null,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &gctx->surface.present_semaphore,
        .pWaitDstStageMask    = &wait_stage,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &gctx->surface.render_semaphore,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd
    };

    res = vkQueueSubmit (
        gctx->device.graphics_queue.handle,
        1,
        &submit_info,
        gctx->surface.render_fence
    );
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to submit command buffers for execution. RET = %d\n",
        res
    );

    VkPresentInfoKHR present_info = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = Null,
        .swapchainCount     = 1,
        .pSwapchains        = &gctx->surface.swapchain,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &gctx->surface.render_semaphore,
        .pImageIndices      = &next_image_index
    };

    res = vkQueuePresentKHR (gctx->device.graphics_queue.handle, &present_info);
    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
        RETURN_VALUE_IF (
            !surface_recreate_swapchain (&gctx->surface, &gctx->device, xwin),
            False,
            "Failed to recreate swapchain\n"
        );
        res = VK_SUCCESS; /* so that next check does not fail */
    }
    RETURN_VALUE_IF (
        res != VK_SUCCESS,
        False,
        "Failed to present rendered images to surface. RET = %d\n",
        res
    );
    return True;
}

/* Describe callbacks in graphics plugin data */
static XuiGraphicsPlugin vulkan_graphics_plugin_data = {
    .context_create  = graphics_context_create,
    .context_destroy = graphics_context_destroy,
    .draw_2d         = draw_2d,
    .draw_indexed_2d = Null
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
