#version 460 core

out vec4 FragColor;

in vec3 frag_world_pos;
in vec3 frag_normal;

layout(binding = 3) uniform sampler3D smoke_read_tex;

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

void main () {

    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    vec3 scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    vec3 light_dir = normalize(light_direction.xyz);

    vec3 N = normalize(frag_normal);
    vec3 L = light_dir;
    float diffuse_factor = max(dot(N,L), 0.0);

    float shadow_step_size = step_size * shadow_step_factor;
    float total_light_density = 0.0;

    vec3 shadow_pos = frag_world_pos;

    for(int i = 0; i < shadow_steps; i++) {
        shadow_pos += light_dir * shadow_step_size;
        if(any(lessThan(shadow_pos, vec3(0.0))) || any(greaterThan(shadow_pos, vec3(1.0)))) break;

        vec3 shadow_tex = shadow_pos / vec3(grid_size.xyz);
        vec3 shadow_rgb = texture(smoke_read_tex, shadow_tex).rgb;
        total_light_density += (shadow_rgb.r + shadow_rgb.g + shadow_rgb.b);
    }

    float light_attenuation = exp(-total_light_density * shadow_density_factor * shadow_step_size);

    vec3 final_light = ambient_color.xyz + (light_color.xyz * diffuse_factor * light_attenuation);
    vec3 final_color = objects_albedo.xyz * final_light;

    FragColor = vec4(final_color, 1.0f);
}