#version 450
#extension GL_EXT_scalar_block_layout : enable

layout (location = 0) in vec2 in_position;
layout (location = 0) out vec4 out_color;

layout (std140, set = 0, binding = 0) uniform UiData {
    vec4  color;
    vec3  position;
    vec2  size;
    int ignore;
} ui_data;

void main() {
    gl_Position = vec4 (
        ui_data.position.x + (in_position.x * ui_data.size.x), /* x */
        ui_data.position.y + (in_position.y * ui_data.size.y), /* y */
        ui_data.position.z,                                    /* z */
        1.0f                                                   /* w */
    );
    out_color = ui_data.color;
}
