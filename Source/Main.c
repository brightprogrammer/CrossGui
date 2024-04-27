#include <Anvie/Common.h>
#include <Anvie/Types.h>

/* crosswindow */
#include <Anvie/CrossWindow/Event.h>
#include <Anvie/CrossWindow/Vulkan.h>
#include <Anvie/CrossWindow/Window.h>

/* crossgui */
#include <Anvie/CrossGui/Graphics.h>
#include <Anvie/CrossGui/Utils/Vector.h>

/* libc */
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

/**************************************************************************************************/
/********************************************** Math **********************************************/
/**************************************************************************************************/

static const Vertex2D triangle_vertices[] = {
    {{-1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
    {  {1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},
    { {-1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},

    {  {1.f, 1.f}, {.6f, .8f, 1.f, 1.f}},
    {{-1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
    { {1.f, -1.f}, {.6f, .8f, 1.f, 1.f}},
};

/**************************************************************************************************/
/************************************************  ************************************************/
/**************************************************************************************************/

int main() {
    const Uint32 width  = 360;
    const Uint32 height = 240;
    XwWindow    *win    = xw_window_create ("Ckeckl", width, height, 10, 20);

    Vulkan *vk = vk_create();
    GOTO_HANDLER_IF (!vk, VK_INIT_FAILED, "Failed to create Vulkan\n");

    Device *device = device_create (vk);
    GOTO_HANDLER_IF (!device, DEVICE_FAILED, "Failed to create Device\n");

    Surface *surface = surface_create (vk, device, win);
    GOTO_HANDLER_IF (!surface, SURFACE_FAILED, "Failed to create Surface\n");

    DeviceBuffer *vbo = device_buffer_create (
        device,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        sizeof (triangle_vertices),
        device->graphics_queue.family_index
    );
    GOTO_HANDLER_IF (!vbo, DEVICE_BUFFER_FAILED, "Failed to create DeviceBuffer\n");

    device_buffer_memcpy (vbo, device, (void *)triangle_vertices, sizeof (triangle_vertices));

    ShaderResourceBinding *ui_srb = shader_resource_binding_create_ui_binding (device);
    GOTO_HANDLER_IF (!ui_srb, UI_SRB_FAILED, "Failed to create UI SRB\n");

    ShaderPipeline *ui_pipeline = shader_pipeline_create_ui_pipeline (device, surface, ui_srb);
    GOTO_HANDLER_IF (!ui_pipeline, UI_PIPELINE_FAILED, "Failed to create UI Pipeline");

    GpuUiData ui_data = {.button_width = 0.25f, .button_height = 0.10f};
    device_buffer_memcpy (ui_srb->uniform_buffer, device, &ui_data, sizeof (GpuUiData));

    /* event handlign looop */
    Bool    is_running = True;
    XwEvent e;
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
    }

    shader_pipeline_destroy (ui_pipeline, device);
    shader_resource_binding_destroy (ui_srb, device);
    device_buffer_destroy (vbo, device);
    surface_destroy (surface, device, vk);
    device_destroy (device);
    vk_destroy (vk);
    xw_window_destroy (win);

    return EXIT_SUCCESS;

DRAW_ERROR:
    shader_pipeline_destroy (ui_pipeline, device);
UI_PIPELINE_FAILED:
    shader_resource_binding_destroy (ui_srb, device);
UI_SRB_FAILED:
    device_buffer_destroy (vbo, device);
DEVICE_BUFFER_FAILED:
    surface_destroy (surface, device, vk);
SURFACE_FAILED:
    device_destroy (device);
DEVICE_FAILED:
    vk_destroy (vk);
VK_INIT_FAILED:
    xw_window_destroy (win);
    return EXIT_FAILURE;
}
