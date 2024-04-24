/* local includes */
#include "GraphicsContext.h"

extern Vulkan vk;

/**
 * @b Create graphics context for Vulkan plugin.
 *
 * @param xwin @c Window to create graphics context for.
 * 
 * @return @c XuiGraphicsContext pointer on success.
 * @return @c Null otherwise.
 * */
XuiGraphicsContext *graphics_context_create (XwWindow *xwin) {
    RETURN_VALUE_IF (!xwin, Null, ERR_INVALID_ARGUMENTS);

    XuiGraphicsContext *gctx = NEW (XuiGraphicsContext);
    RETURN_VALUE_IF (!gctx, Null, ERR_OUT_OF_MEMORY);

    GOTO_HANDLER_IF (
        !device_init (&gctx->device, &vk) ||
            !surface_init (&gctx->surface, &gctx->device, &vk, xwin),
        GCTX_FAILED,
        "Failed to create graphics context\n"
    );

    return gctx;

GCTX_FAILED:
    graphics_context_destroy (gctx);
    return Null;
}

/**
 * @b Destroy the given @x XuiGraphicsContext object.
 *
 * @param gctx @c XuiGraphicsContext object to be destroyed. 
 * */
void graphics_context_destroy (XuiGraphicsContext *gctx) {
    RETURN_IF (!gctx, ERR_INVALID_ARGUMENTS);

    surface_deinit (&gctx->surface, &gctx->device, &vk);
    device_deinit (&gctx->device);

    FREE (gctx);
}

/**
 * @b Resize the graphics context if window is resized.
 *
 * User must necessarily issue a call to this method, if window is resized.
 * 
 * @param gctx Graphics context to be resized.
 * @param xwin Window associated with this graphics context and was resized.
 *
 * @return True on success.
 * @return False otherwise.
 * */
Bool graphics_context_resize (XuiGraphicsContext *gctx, XwWindow *xwin) {
    RETURN_VALUE_IF (!gctx || !xwin, False, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        !surface_recreate_swapchain (&gctx->surface, &gctx->device, xwin),
        False,
        "Failed to resize graphics context.\n"
    );

    return True;
}
