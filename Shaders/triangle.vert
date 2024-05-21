#version 450
#extension GL_EXT_scalar_block_layout : enable

/* sent my mesh data buffer */
/* per vertex */
layout (location = 0) in vec2 mesh_vtx_pos;

/* sent by batch buffer containing mesh instance data */
/* per instance */
layout (location = 1) in uint instance_mesh_type;
layout (location = 2) in vec2 instance_scale;
layout (location = 3) in vec3 instance_pos;
layout (location = 4) in vec4 instance_color;

layout (location = 0) out vec4 out_color;

void main() {
    gl_Position = vec4 (
        instance_pos.x + (mesh_vtx_pos.x * instance_scale.x), /* x */
        - instance_pos.y - (mesh_vtx_pos.y * instance_scale.y), /* y */
        instance_pos.z,                                       /* z */
        1.0f                                                  /* w */
    );

    out_color = instance_color;
}
