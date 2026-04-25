#version 460 core

layout(binding = 3) uniform sampler2D smoke_read_tex;

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

out vec4 FragColor;

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

//rudimentary

void main() {

    vec2 tex_coord = gl_FragCoord.xy / draw_area.zw;
    vec3 smoke_raw = texture(smoke_read_tex, tex_coord, 0).xyz;

    float raw_density = max(smoke_raw.r, max(smoke_raw.g, smoke_raw.b));
    float alpha = 1.0 - exp(-raw_density * smoke_density_factor);

    vec3 smoke_color = vec3(0.0);
    if(raw_density > 0.001) {
        smoke_color = smoke_raw / raw_density;
    }

    vec3 color = mix(ambient_color.rgb, smoke_color, alpha);

    FragColor = vec4(color, 1.0);
}