#include "Anvie/CrossGui/Plugin/Graphics/API.h"
#include "Anvie/CrossGui/Plugin/Graphics/Graphics.h"

#include <Anvie/Common.h>
#include <Anvie/CrossWindow/Event.h>
#include <Anvie/CrossWindow/Window.h>
#include <Anvie/Types.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Plugin.h>

/* for plugin loading */
#include <dlfcn.h>

XuiPlugin *xui_plugin_load (CString plugin_name) {
    RETURN_VALUE_IF (!plugin_name, Null, ERR_INVALID_ARGUMENTS);

    void *plugin_handle = dlopen (plugin_name, RTLD_NOW);
    RETURN_VALUE_IF (!plugin_handle, Null, "Failed to open plugin %s\n", plugin_name);

    XuiPlugin *plugin = (XuiPlugin *)dlsym (plugin_handle, "xui_plugin");
    GOTO_HANDLER_IF (
        !plugin,
        PLUGIN_NOT_FOUND,
        "Failed to find plugin info \"xui_plugin\". Did you define it publicly in the plugin?\n"
    );

    plugin->plugin_handle = plugin_handle;

    PRINT_ERR (
        "Loaded plugin \"%s\" Version %u.%u.%u\n",
        plugin->name,
        plugin->version.date,
        plugin->version.month,
        plugin->version.year
    );

    return plugin;
PLUGIN_NOT_FOUND:
    dlclose (plugin_handle);
    return Null;
}

void xui_plugin_unload (XuiPlugin *plugin) {
    RETURN_IF (!plugin, ERR_INVALID_ARGUMENTS);

    dlclose (plugin->plugin_handle);
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

    XwEvent e;
    Bool    is_running = True;
    while (is_running) {
        while (xw_event_poll (&e)) {
            is_running = e.type != XW_EVENT_TYPE_CLOSE_WINDOW;
        }
        gplug->draw_2d (gctx, xwin, (Vertex2D *)1, 1);
    }

    /* TODO: port SRB objects as well to vulkan plugin.
     * We need some way to create graphics pipeline using plugins. 
     * Need to find some way to link SRB and Pipeline.
     * It is possible to keep SRB and Pipeline as opaque objects, as pipeline is just a 
     * bunch of shaders executed in a particular order, we can emulate it in any graphics API.
     * SRB is also possible in opaque manner.
     * */

    gplug->context_destroy (gctx);
    xw_window_destroy (xwin);
    plugin->deinit();

    xui_plugin_unload (plugin);

    return EXIT_SUCCESS;
}
