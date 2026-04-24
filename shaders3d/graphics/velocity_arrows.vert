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
    float _pad0;
    float _pad1;
    float cell_size;
};

uniform int direction; //x = 0; y = 1; z = 2;

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

flat out float velocity_value;

vec3 get_world_posx(ivec3 pos, vec3 cell_width) {
    vec3 world_pos = vec3(pos.x, pos.y + 0.5f, pos.z + 0.5f);
    world_pos *= cell_width;
    return world_pos;
}

vec3 get_world_posy(ivec3 pos, vec3 cell_width) {
    vec3 world_pos = vec3(pos.x + 0.5f, pos.y, pos.z + 0.5f);
    world_pos *= cell_width;
    return world_pos;
}

vec3 get_world_posz(ivec3 pos, vec3 cell_width) {
    vec3 world_pos = vec3(pos.x + 0.5f, pos.y + 0.5f, pos.z);
    world_pos *= cell_width;
    return world_pos;
}

void main() {
    int idx = gl_InstanceID;

    ivec3 dims;

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    if(direction == 0) {
        dims = ivec3(grid_size.x + 1, grid_size.yz);
    } else if(direction == 1) {
        dims = ivec3(grid_size.x, grid_size.y + 1, grid_size.z);
    } else {
        dims = ivec3(grid_size.xy, grid_size.z + 1);
    }

    ivec3 arrow_index = ivec3(idx % dims.x, (idx / dims.x) % dims.y , idx / (dims.x * dims.y));

    vec3 world_pos;
    vec3 cell_width = scaled_grid_size / vec3(grid_size.xyz);
    if(direction == 0) {
        velocity_value = texelFetch(vx_read_tex, arrow_index, 0).r;
        world_pos = get_world_posx(arrow_index, cell_width);
    } else if(direction == 1) {
        velocity_value = texelFetch(vy_read_tex, arrow_index, 0).r;
        world_pos = get_world_posy(arrow_index, cell_width);
    } else {
        velocity_value = texelFetch(vz_read_tex, arrow_index, 0).r;
        world_pos = get_world_posz(arrow_index, cell_width);
    }

    const float ARROW_SCALE = 0.01f;
    vec3 scaled_arrow = arrow_pos * ARROW_SCALE;

    if(velocity_value < 0) {
        if(direction == 0) scaled_arrow *= -1.0;
        else if(direction == 1) scaled_arrow.y *= -1.0f;
        else scaled_arrow.z *= -1.0f;
    }

    const float ARROW_MIN = 0.001;

    if(abs(velocity_value) < ARROW_MIN) {
        scaled_arrow = vec3(0);
        world_pos = vec3(0);
    }

    gl_Position = projection * look_at * vec4(scaled_arrow + world_pos, 1.0);
}