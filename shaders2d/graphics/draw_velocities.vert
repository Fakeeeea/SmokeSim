#version 460 core

layout (location = 0) in vec2 pos;

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

uniform int direction;

vec2 get_world_posx(ivec2 pos, vec2 cell_width) {
    vec2 world_pos = vec2(pos.x, pos.y + 0.5f);
    world_pos *= cell_width;
    return world_pos;
}

vec2 get_world_posy(ivec2 pos, vec2 cell_width) {
    vec2 world_pos = vec2(pos.x + 0.5f, pos.y);
    world_pos *= cell_width;
    return world_pos;
}

flat out float velocity_value;

void main() {
    int idx = gl_InstanceID;

    ivec2 dims;

    if(direction == 0) {
        dims = ivec2(grid_size.x + 1, grid_size.y);
    } else {
        dims = ivec2(grid_size.x, grid_size.y + 1);
    }

    ivec2 arrow_index = ivec2(idx % dims.x, (idx / dims.x));

    vec2 world_pos;
    vec2 cell_width = 1.0 / vec2(grid_size.xy);

    if(direction == 0) {
        velocity_value = texelFetch(vx_read_tex, arrow_index, 0).r;
        world_pos = get_world_posx(arrow_index, cell_width);
    } else {
        velocity_value = texelFetch(vy_read_tex, arrow_index, 0).r;
        world_pos = get_world_posy(arrow_index, cell_width);
    }

    const float ARROW_SCALE = 0.01f;
    vec2 scaled_arrow = pos * ARROW_SCALE;

    if(velocity_value < 0) {
        if(direction == 0) scaled_arrow *= -1.0;
        else scaled_arrow.y *= -1.0f;
    }

    const float ARROW_MIN = 0.1;

    if(abs(velocity_value) < ARROW_MIN) {
        scaled_arrow = vec2(0);
        world_pos = vec2(0);
    }

    vec2 clip_pos = (scaled_arrow + world_pos) * 2.0 - 1.0;

    gl_Position =  vec4(clip_pos, -1.0, 1.0);
}