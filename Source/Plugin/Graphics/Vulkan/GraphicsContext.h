#ifndef ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H
#define ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H

/* local includes */
#include "Device.h"
#include "Surface.h"

typedef struct XuiGraphicsContext {
    Device  device;
    Surface surface;
} XuiGraphicsContext;

XuiGraphicsContext *graphics_context_create (XwWindow *xwin);
void                graphics_context_destroy (XuiGraphicsContext *gctx);
Bool                graphics_context_resize (XuiGraphicsContext *gctx, XwWindow *xwin);

#endif // ANVIE_CROSSGUI_SOURCE_PLUGIN_GRAPHICS_VULKAN_GRAPHICS_CONTEXT_H
