#version 460 core

layout(binding = 0) uniform sampler3D vx_read_tex;
layout(binding = 1) uniform sampler3D vy_read_tex;
layout(binding = 2) uniform sampler3D vz_read_tex;
layout(binding = 3) uniform sampler3D smoke_read_tex;
layout(binding = 4) uniform sampler2D depth_texture;

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

in vec3 vertex_pos; //raw box coordinates
in vec4 screen_pos; //with applied projection and lookat matrix

vec2 uv_space;
vec2 ndc;
vec3 scaled_grid_size;

const float PI = 3.14;

float sample_depth(vec2 uv) {
    return texture(depth_texture, uv).r;
}

vec3 get_direction_from_camera() {
    ndc = (screen_pos.xy / screen_pos.w);
    uv_space =  gl_FragCoord.xy / vec2(textureSize(depth_texture, 0));
    vec4 ray_clip = vec4(ndc, -1.0, 1.0);
    vec4 ray_view = inverse(projection) * ray_clip;
    ray_view /= ray_view.w;

    vec3 ray_origin = camera_pos.xyz;
    return normalize((inverse(look_at) * ray_view).xyz - ray_origin).xyz;
}

float get_distance_from_obstacle(vec2 uv) {
    float depth_val = sample_depth(uv);
    float depth_ndc = depth_val * 2.0 - 1.0;

    vec4 clip_pos = vec4(ndc.xy, depth_ndc, 1.0);
    vec4 camera_space = inverse(projection) * clip_pos;
    camera_space /= camera_space.w;

    return length(camera_space.xyz);
}

vec2 intersect_aabb(vec3 origin, vec3 dir, vec3 box_min, vec3 box_max) {
    vec3 dir_reciprocal = 1.0 / dir;
    vec3 t0 = (box_min - origin) * dir_reciprocal;
    vec3 t1 = (box_max - origin) * dir_reciprocal;
    vec3 intersect_min = min(t0, t1);
    vec3 intersect_max = max(t0, t1);
    float ray_enter = max(intersect_min.x, max(intersect_min.y, intersect_min.z));
    float ray_exit = min(intersect_max.x, min(intersect_max.y, intersect_max.z));
    return vec2(ray_enter, ray_exit);
}

vec2 get_ray_enter_exit(vec3 ray_direction) {

    vec2 ray_enter_exit = intersect_aabb(camera_pos.xyz, ray_direction, vec3(0), scaled_grid_size);

    float dist_from_obstacle = get_distance_from_obstacle(uv_space);

    float ray_limit_obstacle = min(ray_enter_exit.g, dist_from_obstacle);

    float ray_max = min(ray_limit_obstacle, ray_enter_exit.g);

    if(ray_max < max(ray_enter_exit.r, 0.0)) {
        discard;
    }

    return vec2(ray_enter_exit.r, ray_max);
}

float phase_function(const float g, const float cos_theta) {
    if(g == 0) return 1.0 / PI;
    float denominator = 1.0 + g * g - 2.0 * g * cos_theta;
    return 1.0 / (4.0 * PI) * (1.0 - g * g) / (denominator * sqrt(denominator));
}

void main() {
    float min_dim = min(grid_size.x, min(grid_size.y, grid_size.z));
    scaled_grid_size = vec3(grid_size.xyz) / min_dim;

    vec3 ray_direction = get_direction_from_camera();
    vec2 enter_exit_distances = get_ray_enter_exit(ray_direction);

    vec3 light_dir = normalize(light_direction.xyz);
    float transmittance = 1.0;
    vec3 color = vec3(0.0);

    int steps = int((enter_exit_distances.g - enter_exit_distances.r) / step_size) + 1;
    float noise = fract(sin(dot(uv_space, vec2(12.9898, 78.233))) * 43758.5453);
    float t = enter_exit_distances.r + noise * step_size;

    float g = 0.5;
    vec3 view_dir = -ray_direction;
    float phase = phase_function(g, dot(view_dir, light_dir)) * 3;

    for(int i = 0; i < steps; ++i) {
        if (t > enter_exit_distances.g) break;

        vec3 pos = camera_pos.xyz + ray_direction * t;
        vec3 tex_coords = pos / scaled_grid_size;

        vec3 density = texture(smoke_read_tex, tex_coords).rgb;
        float total_density = density.r + density.g + density.b;

        if (total_density > 0.001) {
            vec3 smoke_albedo = density / total_density;

            float light_density = 0.0;
            float shadow_step_size = step_size * shadow_step_factor;
            vec3 light_pos = pos;

            for (int j = 0; j < shadow_steps; j++) {
                light_pos += light_dir * shadow_step_size;

                vec3 shadow_tex_coord = light_pos / scaled_grid_size;
                vec3 shadow_rgb = texture(smoke_read_tex, shadow_tex_coord).rgb;
                light_density += (shadow_rgb.r + shadow_rgb.g + shadow_rgb.b);
            }

            float light_attenuation = exp(-light_density * shadow_density_factor * shadow_step_size);
            vec3 current_light = ambient_color.xyz + (light_color.xyz * light_attenuation) * phase;
            //vec3 current_light = ambient_color.xyz + (light_color.xyz * light_attenuation);

            float alpha = 1.0 - exp(-total_density * smoke_density_factor * step_size);

            color += transmittance * alpha * (smoke_albedo * current_light);
            transmittance *= (1.0 - alpha);

            if (transmittance < 0.001) {
                break;
            }
        }

        t += step_size;
    }

    FragColor = vec4(color, 1.0 - transmittance);
}