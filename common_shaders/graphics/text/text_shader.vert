#version 460 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 uv;

out vec2 TexCoords;

layout(std140, binding = 0) uniform graphics_variables {
    mat4 look_at;
    mat4 projection;
    mat4 ortho;
    vec4 camera_pos;
    vec4 ambient_color;
    vec4 objects_albedo;
    vec4 light_color;
    vec4 light_direction;
    float smoke_density_factor;
    float shadow_density_factor;
    float step_size;
    float shadow_step_factor;
    float shadow_steps;
    float z_near;
    float z_far;
    float fov;
};

uniform float scale;
uniform vec2 t_offset;
uniform vec2 pivot;

void main() {
    vec2 pos = ((vertex.xy - pivot) * scale) + pivot + t_offset;
    gl_Position = ortho * vec4(pos, 0.0, 1.0);
    TexCoords = uv;
}