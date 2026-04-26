#version 460 core

layout (location = 0) in vec2 pos;

layout(binding = 0) uniform sampler2D vx_read_tex;
layout(binding = 1) uniform sampler2D vy_read_tex;

layout(binding = 6, r32f) uniform image3D vorticity;

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

flat out float mag;

vec2 get_interpolated_velocity(vec2 world_pos) {
    vec2 v;

    vec2 vx_uv = (world_pos + vec2(0.5, 0)) / (vec2(grid_size.xy) + vec2(1, 0));
    vec2 vy_uv = (world_pos + vec2(0, 0.5)) / (vec2(grid_size.xy) + vec2(0, 1));

    v.x = texture(vx_read_tex, vx_uv).r;
    v.y = texture(vy_read_tex, vy_uv).r;

    return v;
}

void main() {
    int idx = gl_InstanceID;

    ivec2 arrow_index = ivec2(idx % grid_size.x, idx / grid_size.x);

    vec2 vel = get_interpolated_velocity(vec2(arrow_index) + 0.5f);

    mag = length(vel);

    const float MIN_MAG = 0.2;

    if(mag < MIN_MAG) {
        gl_Position = vec4(10.0,10.0,0.0,1.0);
        return;
    }

    vec2 vec_norm = vel / mag;

    mat2 R = mat2(vec_norm.x, vec_norm.y, -vec_norm.y, vec_norm.x);
    vec2 length_scaled = vec2(clamp(pos.x * mag, pos.x, pos.x * 3), pos.y);
    vec2 rotated_vertex = R * length_scaled;


    vec2 cell_width = draw_area.zw / vec2(grid_size.xy);

    vec2 scale = cell_width * 1.0f;
    vec2 scaled_vertex = rotated_vertex * scale;

    vec2 final_pos = scaled_vertex + ((vec2(arrow_index) + 0.5f)) * cell_width;
    vec2 clip_space = (final_pos / draw_area.zw) * 2.0 - 1.0;

    gl_Position = vec4(clip_space, -1.0f, 1.0f);
}