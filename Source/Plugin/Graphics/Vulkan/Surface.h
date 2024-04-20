#ifndef ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H
#define ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H

/* local includes */
#include "Device.h"

typedef struct Vulkan   Vulkan;
typedef struct XwWindow XwWindow;

typedef struct Surface {
    VkSurfaceKHR surface;                  /**< @b Surface created for corrsponding XwWindow. */

    VkSwapchainKHR swapchain;              /**< @b Swapchain created for the window. */
    VkExtent2D     swapchain_image_extent; /**< @b Current swapchain image extent */
    VkFormat swapchain_image_format;    /**< @b Format of image stored during swapchain creation. */
    Uint32   swapchain_image_count;     /**< @b Number of images in swapchain. */
    VkImage *swapchain_images;          /**< @b Handle to images inside swapchain. */
    VkImageView *swapchain_image_views; /**< @b Image views created for images in swapchain. */

    VkCommandPool   cmd_pool;
    VkCommandBuffer cmd_buffer;

    VkRenderPass   render_pass;
    VkFramebuffer *framebuffers;

    VkSemaphore render_semaphore;
    VkSemaphore present_semaphore;
    VkFence     render_fence;
} Surface;

Surface *surface_init (Surface *surf, Device *device, Vulkan *vk, XwWindow *xw_win);
Surface *surface_deinit (Surface *surf, Device *device, Vulkan *vk);
Surface *surface_recreate_swapchain (Surface *surf, Device *device, XwWindow *win);

#endif // ANVIE_SOURCE_CROSSGUI_PLUGIN_GRAPHICS_VULKAN_SURFACE_H
