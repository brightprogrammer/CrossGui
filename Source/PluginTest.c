#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* crosswindow */
#include <Anvie/CrossWindow/Event.h>
#include <Anvie/CrossWindow/Window.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Graphics/API.h>
#include <Anvie/CrossGui/Plugin/Graphics/Graphics.h>
#include <Anvie/CrossGui/Plugin/Plugin.h>

/* for plugin loading */
#include <dlfcn.h>

void draw_ui (XuiGraphicsPlugin *gplug, XuiGraphicsContext *gctx, XwWindow *xwin) {
    RETURN_IF (!gplug || !gctx || !xwin, ERR_INVALID_ARGUMENTS);

    gplug->clear (gctx, xwin);

    if (!gplug->draw_rect_2d (
            gctx,
            xwin,
            (Rect2D) {
                .position = {0, 0},
                .scale    = {0.1, 0.05},
                .color    = {1, 1, 1, 1},
                .depth    = 0.f
    }
        )) {
        return;
    }

    if (!gplug->draw_rect_2d (
            gctx,
            xwin,
            (Rect2D) {
                .position = {0, 0.1},
                .scale    = {0.1, 0.05},
                .color    = {1, 0, 0, 1},
                .depth    = 0.f
    }
        )) {
        return;
    }
}

int main (Int32 argc, CString *argv) {
    RETURN_VALUE_IF (argc < 2, EXIT_FAILURE, "%s <plugin path>\n", argv[0]);

    XuiPlugin *plugin = xui_plugin_load (argv[1]);
    RETURN_VALUE_IF (!plugin, EXIT_FAILURE, "Failed to load plugin\n");

    plugin->init();
    XuiGraphicsPlugin *gplug = (XuiGraphicsPlugin *)plugin->plugin_data;

    XwWindow           *xwin = xw_window_create (Null, 540, 360, 0, 0);
    XuiGraphicsContext *gctx = gplug->context_create (xwin);
    RETURN_VALUE_IF (!gctx, EXIT_FAILURE, "Failed to create graphics context\n");

    draw_ui (gplug, gctx, xwin);

    XwEvent e;
    Bool    is_running = True;
    while (is_running) {
        Bool resized = False;
        while (xw_event_poll (&e)) {
            switch (e.type) {
                case XW_EVENT_TYPE_CLOSE_WINDOW : {
                    is_running = False;
                    break;
                }
                case XW_EVENT_TYPE_RESIZE : {
                    resized = True;
                    break;
                }
                default :
                    break;
            }
        }

        if (resized) {
            gplug->context_resize (gctx, xwin);
            draw_ui (gplug, gctx, xwin);
        }
    }

    gplug->context_destroy (gctx);
    xw_window_destroy (xwin);
    plugin->deinit();

    xui_plugin_unload (plugin);

    return EXIT_SUCCESS;
}
