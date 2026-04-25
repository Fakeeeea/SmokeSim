#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D vx_read_tex;
layout(binding = 1) uniform sampler2D vy_read_tex;

layout(std140, binding = 1) uniform constants {
    ivec4 grid_size;
    vec4 wind;
    float time_step;
    float density;
    float t_ambient;
    float t_weight_coeff;
    float t_temp_coeff;
    float vorticity_confinement;
    float _pad0;
    float _pad1;
    float cell_size;
};

layout(std140, binding = 0) uniform graphics_variables {
    mat4 look_at;
    mat4 projection;
    mat4 ortho;
    vec4 draw_area;
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

flat in float velocity_value;

void main() {
    float is_negative = velocity_value < 0 ? 1 : 0;
    float intensity = abs(velocity_value);
    FragColor = vec4(intensity * is_negative, intensity * (1 - is_negative), 0.5f, 1.0f);
}