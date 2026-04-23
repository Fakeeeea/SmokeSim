#version 460 core

layout (location = 0) in vec3 pos;

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

flat out int type;

void main() {

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = grid_size / min_dim;

    if(pos.x > 0) type = 0;
    if(pos.y > 0) type = 1;
    if(pos.z > 0) type = 2;


    gl_Position = projection * look_at * vec4(pos*scaled_grid_size*1.05f,1.0);
}