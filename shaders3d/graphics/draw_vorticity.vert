#version 460 core

layout (location = 0) in vec3 arrow_pos;

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

flat out float mag;

mat3 rotation_from_to(vec3 a, vec3 b) {
    float c = dot(a, b);
    vec3 v = cross(a, b);

    float s = length(v);

    mat3 vx = mat3(0, -v.z, v.y,
    v.z, 0, -v.x,
    -v.y, v.x, 0);

    return mat3(1.0) + vx + vx * vx * ((1.0 - c) / (s*s));
}

void main() {
    int idx = gl_InstanceID;

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    ivec3 arrow_index = ivec3(idx % grid_size.x, (idx / grid_size.x) % grid_size.y , idx / (grid_size.x * grid_size.y));

    vec3 vort = imageLoad(vorticity, arrow_index).xyz;

    mag = length(vort);
    if(mag < 0.5) {
        gl_Position = vec4(0.0,0.0,0.0,0.0);
        return;
    }

    vec3 base_dir = vec3(1,0,0);
    vec3 vorticity_norm = normalize(vort);

    mat3 R = rotation_from_to(base_dir, vorticity_norm);

    vec3 length_scaled = vec3(clamp(arrow_pos.x * mag, arrow_pos.x, arrow_pos.x * 3), arrow_pos.y, arrow_pos.z);
    vec3 rotated_vertex = R * length_scaled;

    float scale = 0.005f;

    vec3 scaled_vertex = scale * rotated_vertex;

    vec3 cell_width = scaled_grid_size / vec3(grid_size.xyz);

    gl_Position = projection * look_at * vec4(scaled_vertex + ((arrow_index + 0.5f) * cell_width), 1.0f);
}