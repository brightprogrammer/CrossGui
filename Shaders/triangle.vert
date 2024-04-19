#version 450

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec4 in_color;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform UiData {
    float button_width;
    float button_height;
} ui;

void main() {
    gl_Position = vec4 (in_pos.x * ui.button_width, in_pos.y * ui.button_height, 0.f, 1.0f);
    out_color = in_color;
}
