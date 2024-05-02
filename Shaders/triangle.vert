#version 450

layout (location = 0) in vec2 in_pos;

layout (location = 0) out vec4 out_color;

void main() {
    /* TODO: get depth value in uniform */
    gl_Position = vec4 (in_pos.x *0.5f, in_pos.y * 0.5f, 0.f, 1.0f);
    out_color = vec4(1, 1, 1, 1);
}
