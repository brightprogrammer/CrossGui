#version 450

layout (location = 0) in vec2 in_position;
layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform UiData {
    vec2  position;
    vec2  size;
    vec4  color;
    float depth;
} ui_data;

void main() {
    gl_Position = vec4 (
        ui_data.position.x + (in_position.x * ui_data.size.x), /* x */
        ui_data.position.y + (in_position.y * ui_data.size.y), /* y */
        ui_data.depth,                                       /* z */
        1.0f                                                 /* w */
    );
    out_color = ui_data.color;
}
