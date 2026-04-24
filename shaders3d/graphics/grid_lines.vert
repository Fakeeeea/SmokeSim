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

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    float row_lenght;
    if(direction == 0) {
        row_lenght = grid_size.y + 1;
    } else if (direction == 1) {
        row_lenght = grid_size.x + 1;
    } else {
        row_lenght = grid_size.x + 1;
    }

    ivec2 arrow_id = ivec2(gl_InstanceID % int(row_lenght),
                            gl_InstanceID / int(row_lenght));

    vec3 spacing = scaled_grid_size;
    vec3 traslated_pos = pos * scaled_grid_size;
    spacing /= vec3(grid_size.xyz);

    if(direction == 0) {
        traslated_pos += vec3(0.0f, spacing.y * arrow_id.x, spacing.z * arrow_id.y);
    } else if (direction == 1) {
        traslated_pos += vec3(spacing.x * arrow_id.x, 0.0f, spacing.z * arrow_id.y);
    } else {
        traslated_pos += vec3(spacing.x * arrow_id.x, spacing.y * arrow_id.y, 0.0f);
    }

    gl_Position = projection * look_at * vec4(traslated_pos,1.0);
}