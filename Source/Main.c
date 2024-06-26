#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* crosswindow */
#include <Anvie/CrossWindow/Event.h>
#include <Anvie/CrossWindow/Window.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Graphics/Graphics.h>
#include <Anvie/CrossGui/Plugin/Plugin.h>

typedef enum MeshType {
    MESH_TYPE_RECTANGLE,
    MESH_TYPE_TRIANGLE1,
    MESH_TYPE_TRIANGLE2,
    MESH_TYPE_TRIANGLE3,
    MESH_TYPE_LINE,
} MeshType;

void draw_ui (XuiGraphicsPlugin *gplug, XuiGraphicsContext *gctx, XwWindow *xwin) {
    RETURN_IF (!gplug || !gctx || !xwin, ERR_INVALID_ARGUMENTS);
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = 0, .y = 0, .z = 1.f},
                .scale    = {.x = 1, .y = 1},
                .color    = {.r = 0.9, .g = 0.9, .b = 0.8, .a = 1},
    }
        )) {
        return;
    }

    /* left panel */
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = -0.5 - 0.17, .y = 0.0, .z = 0.9f},
                .scale    = {.x = 0.3, .y = 0.95},
                .color    = {.r = 0.5, .g = 0.5, .b = 0.4, .a = 1},
    }
        )) {
        return;
    }

    /* right panel */
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = 0.32, .y = 0.0, .z = 0.9f},
                .scale    = {.x = 0.65, .y = 0.95},
                .color    = {.r = 0.5, .g = 0.5, .b = 0.4, .a = 1},
    }
        )) {
        return;
    }

    /* right top panel */
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = 0.2, .y = 0.62f, .z = 0.5f},
                .scale    = {.x = 0.5, .y = 0.3},
                .color    = {.r = 0.3, .g = 0.3, .b = 0.2, .a = 1},
    }
        )) {
        return;
    }

    /* right bottom panel */
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = 0.2, .y = -0.32, .z = 0.5f},
                .scale    = {.x = 0.5, .y = 0.60},
                .color    = {.r = 0.3, .g = 0.3, .b = 0.2, .a = 1},
    }
        )) {
        return;
    }

    /* right right panel */
    if (!gplug->draw_2d (
            gctx,
            &(XuiMeshInstance2D) {
                .type     = MESH_TYPE_RECTANGLE,
                .position = {.x = 0.83, .y = 0.0, .z = 0.5f},
                .scale    = {.x = 0.12, .y = 0.92},
                .color    = {.r = 0.3, .g = 0.3, .b = 0.2, .a = 1},
    }
        )) {
        return;
    }

    // /* vertical line */
    // if (!gplug->draw_2d (
    //         gctx,
    //         xwin,
    //         &(XuiMeshInstance2D) {
    //             .type     = MESH_TYPE_RECTANGLE,
    //             .position = {.x = 0.0, .y = 0, .z = 0.f},
    //             .scale    = {.x = 0.005, .y = 1},
    //             .color    = {.r = 0.2, .g = 0.2, .b = 0.2, .a = 1},
    // }
    //     )) {
    //     return;
    // }
    //
    // /* horizontal line */
    // if (!gplug->draw_2d (
    //         gctx,
    //         xwin,
    //         &(XuiMeshInstance2D) {
    //             .type     = MESH_TYPE_RECTANGLE,
    //             .position = {.x = 0.0, .y = 0, .z = 0.f},
    //             .scale    = {.x = 1, .y = 0.005},
    //             .color    = {.r = 0.2, .g = 0.2, .b = 0.2, .a = 1},
    // }
    //     )) {
    //     return;
    // }
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

    XuiMesh2D mesh = {
        .type     = MESH_TYPE_TRIANGLE1,
        .vertices = (Vec2f[]) {{.x = 0.f, .y = 1.f}, {.x = 1.f, .y = 0.f}, {.x = -1.f, .y = 0.f}},
        .vertex_count = 3,
        .indices      = (Uint32[]) {                   0,                    1,                     2},
        .index_count  = 3
    };

    RETURN_VALUE_IF (!gplug->mesh_upload_2d (&mesh), EXIT_FAILURE, "Failed to uplaod mesh data\n");

    XuiMesh2D mesh1 = {
        .type = MESH_TYPE_RECTANGLE,
        .vertices =
            (Vec2f[]) {
                       {.x = -1.f, .y = 1.f},
                       {.x = 1.f, .y = 1.f},
                       {.x = 1.f, .y = -1.f},
                       {.x = -1.f, .y = -1.f},
                       },
        .vertex_count = 4,
        .indices      = (Uint32[]) {0, 1, 2, 2, 3, 0},
        .index_count  = 6
    };

    RETURN_VALUE_IF (!gplug->mesh_upload_2d (&mesh1), EXIT_FAILURE, "Failed to uplaod mesh data\n");

    gplug->clear (gctx, xwin);
    draw_ui (gplug, gctx, xwin);
    gplug->display (gctx, xwin);
    gplug->display (gctx, xwin);

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
            gplug->clear (gctx, xwin);
            gplug->display (gctx, xwin);
        }
    }

    gplug->context_destroy (gctx);
    xw_window_destroy (xwin);
    plugin->deinit();

    xui_plugin_unload (plugin);

    return EXIT_SUCCESS;
}
