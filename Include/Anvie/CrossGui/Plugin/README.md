# CrossGui Plugin System

CrossGui allows a plug-and-play system of doing certain things. For now, this just includes
the graphics backend, but in future, this can extend to various other tasks as well.

## Create A Plugin

Below is an example of how a graphics plugin can be created. Using similar approach, any other
supported/required plugin can be created.


```c 
/* store vulkan state for as long as it's active */
struct {
    VkInstance        instance;
    VkPhysicalDevice *gpus;
    Size              gpu_count;
   
    .
    . /* other data */
    .
} state;

static Bool init() {
    /* initialize graphics state to be used by plugin internally */
    state.instance = create_instance();
    state.gpus = find_gpus(&state.gpu_count);

    .
    .
    .

    /* other initialization work */
}

static Bool deinit() {
    /* deinitialize the state */
}

static Bool draw_2d(Vertex2D *vertices, Size vertex_count) {
    /* drawing code */
}

static Bool draw_indexed_2d(
    Vertex2D *vertices,
    Size vertex_count,
    Uint32 *indices,
    Size index_count
) {
    /* drawing code */
}

/**
 * Create plugin data to be used by CrossGui.

 * This contains everything required by CrossGuo interact 
 * with this specific plugin instance.
 * */
static XuiGraphicsPlugin vulkan_graphics = {
    .init = init,
    .draw_2d = draw_2d,
    .draw_indexed_2d = draw_indexed_2d,
    
    .
    .
    .

    /* other members */
};

/**
 * This name must always be xui_plugin.
 * This name will be used to find the plugin.
 * */
XuiPlugin xui_plugin = {
    .type = XUI_PLUGIN_TYPE_GRAPHICS,
    .name = "Xui Vulkan Graphics",
    .version = {.date = xy, .month = xy, .year = xyzt},
    .license = "Copyright (c) 2024 Siddharth Mishra, Anvie Labs. BSD 3-Clause License",
    .plugin_data = &vulkan_graphics,
    .init = init,
    .deinit = deinit
};
```

The plugin system will then allow the user or CrossGui to select one appropriate plugin based on the 
platform, or user choice. If a platform does not support OpenGL, Vulkan, etc... But supports some other
graphics API is supported, then we move to using that plugin. This allows CrossGui to even have a plugin,
that directly renders to an image on the CPU (something like CairoGraphics).
