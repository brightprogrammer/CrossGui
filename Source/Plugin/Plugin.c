#include <Anvie/Common.h>
#include <Anvie/Platform.h>

/* crossgui */
#include <Anvie/CrossGui/Plugin/Plugin.h>

/* linux loader api */
#include <dlfcn.h>

/* TODO: This is just for linux, add platform-specific code here when required. */

#if HOST_PLATFORM_TYPE != HOST_PLATFORM_TYPE_LINUX
#    error "Host platform is not linux. Other platforms are not supported for now."
#endif

/**
 * @b Load plugin from library file.
 *
 * @param plugin_name Name or path of plugin to be loaded.
 *
 * @return @x XuiPlugin containing plugin information, provided by the plugin 
 *         itself. Do not manage memory for this object as it's owned by the plugin.
 * @return @c Null if plugin is not found.
 * */
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

/**
 * @b Unload given plugin.
 *
 * @param plugin Plugin data returned by @x xui_plugin_load method.
 * */
void xui_plugin_unload (XuiPlugin *plugin) {
    RETURN_IF (!plugin, ERR_INVALID_ARGUMENTS);

    PRINT_ERR ("Unloaded plugin \"%s\"\n", plugin->name);

    dlclose (plugin->plugin_handle);
}
