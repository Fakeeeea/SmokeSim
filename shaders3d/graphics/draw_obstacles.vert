#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

layout(std140, binding = 1) uniform constants {
    vec3 grid_size;
    float _pad0;
    vec3 wind;
    float _pad1;
    float time_step;
    float cell_size;
    float density;
    float t_ambient;
    float t_weight_coeff;
    float t_temp_coeff;
    float vorticity_confinement;
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

struct obstacle {
    vec4 pos_radius;
    vec4 velocities_anchored;
};

layout(std430, binding = 5) buffer obstacle_in {
    int obstacle_count_in;
    obstacle obstacles_in[];
};

out vec3 frag_world_pos;
out vec3 frag_normal;

void main() {
    int obstacle_idx = gl_InstanceID;
    obstacle current = obstacles_in[obstacle_idx];

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = grid_size / min_dim;

    //vec3 world_pos = pos * (current.pos_radius.a / grid_size);
    //world_pos += (current.pos_radius.xyz / grid_size);

    vec3 world_pos = (pos * current.pos_radius.a + current.pos_radius.xyz) / grid_size;
    world_pos *= scaled_grid_size;

    frag_normal = norm;
    frag_world_pos = world_pos;

    gl_Position = projection * look_at * vec4(world_pos, 1.0f);
}