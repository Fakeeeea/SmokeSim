#version 460 core

layout (location = 0) in vec3 pos;

layout(binding = 0) uniform sampler3D vx_read_tex;
layout(binding = 1) uniform sampler3D vy_read_tex;
layout(binding = 2) uniform sampler3D vz_read_tex;
layout(binding = 3) uniform sampler3D smoke_read_tex;

layout(binding = 0, r32f) writeonly uniform image3D vx_write_temp_image;
layout(binding = 1, r32f) writeonly uniform image3D vy_write_temp_image;
layout(binding = 2, r32f) writeonly uniform image3D vz_write_temp_image;
layout(binding = 3, rgba16f) writeonly uniform image3D smoke_write_temp_image;

layout(binding = 4, r32f) uniform image3D pressure_map;
layout(binding = 5, rgba16f) uniform image3D solid_map;
layout(binding = 6, rgba16f) uniform image3D vorticity;

layout(std140, binding = 1) uniform constants {
    ivec4 grid_size;
    vec4 wind;
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

out vec3 vertex_pos;
out vec4 screen_pos;

void main() {

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    vertex_pos = pos * scaled_grid_size;
    screen_pos = projection * look_at * vec4(vertex_pos, 1.0f);
    gl_Position = screen_pos;
}