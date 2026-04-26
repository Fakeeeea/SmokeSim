#version 460 core

layout (location = 0) in vec2 pos;

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

uniform int direction;

void main() {
    int idx = gl_InstanceID;

    vec2 cell_width = 1.0 / vec2(grid_size.xy);
    vec2 new_pos;

    if(direction == 0) {
        new_pos = vec2(pos.x, pos.y + cell_width.y * idx);
    } else {
        new_pos = vec2(pos.x + cell_width.x * idx, pos.y);
    }

    vec2 clip_pos = new_pos * 2.0 - 1.0;

    gl_Position = vec4(clip_pos, -1.0, 1.0);
}