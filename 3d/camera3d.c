//
// Created by aaa on 08/03/2026.
//

#include "camera3d.h"

camera get_camera() {
    camera cam = {
            {0.0f, 0.0f, 3.0f},
            {0.0f, 0.0f, -1.0f},
            {0.0f, 1.0f, 0.0f},
            {-90.0f, 0.0f, 0.0f},
            {1,1,3},
    };
    return cam;
}

void get_lookat(camera cam, mat4 dest) {
    vec3 target;
    glm_vec3_add(cam.camera_pos, cam.camera_front, target);
    glm_lookat(cam.camera_pos,
               target,
               cam.camera_up,
               dest);
}

void move_camera_z(camera* cam, const float amount) {
    vec3 scaled;
    glm_vec3_scale(cam->camera_front, amount, scaled);
    glm_vec3_add(cam->camera_pos, scaled, cam->camera_pos);
}

void mouse_movement(camera* cam, const vec2 offset) {
    if(cam->orbit.orbiting == 1) return;
    const float sens = 0.1f;
    float x_offset_scaled = offset[0] * sens;
    float y_offset_scaled = offset[1] * sens;

    cam->angle[0] += x_offset_scaled;
    cam->angle[1] += y_offset_scaled;

    if(cam->angle[1] > 89.0f) {
        cam->angle[1] = 89.0f;
    }
    if(cam->angle[1] < -89.0f) {
        cam->angle[1] = -89.0f;
    }

    vec3 direction;
    direction[0] = cos(glm_rad(cam->angle[0])) * cos(glm_rad(cam->angle[1]));
    direction[1] = sin(glm_rad(cam->angle[1]));
    direction[2] = sin(glm_rad(cam->angle[0])) * cos(glm_rad(cam->angle[1]));
    glm_normalize(direction);
    glm_vec3_copy(direction, cam->camera_front);
}

void auto_orbit(camera* cam, const ivec3 grid_size, const float time) {

    const float AUTO_ORBIT_ANGLE = 70;

    if(cam->orbit.auto_orbit == 0) return;

    int min_val = min(grid_size[0], min(grid_size[1], grid_size[2]));
    vec3 center; glm_vec3_divs((vec3){(float)grid_size[0], (float)grid_size[1],(float) grid_size[2]}, (float) min_val, center);
    glm_vec3_divs(center, 2, center);

    rotate_camera_around_center(cam, center, glm_rad(70), time);
}

void mouse_movement_orbit(camera* cam, const ivec3 grid_size, const vec2 offset) {
    if(cam->orbit.orbiting == 0) return;

    const float sens = 0.01f;
    float x_offset_scaled = offset[0] * sens;
    float y_offset_scaled = offset[1] * sens;

    cam->angle[0] += x_offset_scaled;
    cam->angle[1] += y_offset_scaled;

    float min_pitch = 0.01f;
    float max_pitch = M_PI - 0.01f;

    if(cam->angle[1] < min_pitch) cam->angle[1] = min_pitch;
    if(cam->angle[1] > max_pitch) cam->angle[1] = max_pitch;

    int min_val = min(grid_size[0], min(grid_size[1], grid_size[2]));
    vec3 center; glm_vec3_divs((vec3){(float)grid_size[0], (float)grid_size[1],(float) grid_size[2]}, (float) min_val, center);
    glm_vec3_divs(center, 2, center);

    rotate_camera_around_center(cam, center, cam->angle[1], cam->angle[0]);
}

void rotate_camera_around_center(camera* cam, const vec3 center, const float angle, const float time) {
    vec3 new_pos;
    new_pos[0] = center[0] + cam->orbit.orbit_distance * sinf(angle) * cosf(time);
    new_pos[1] = center[1] + cam->orbit.orbit_distance * cosf(angle);
    new_pos[2] = center[2] + cam->orbit.orbit_distance * sinf(angle) * sinf(time);
    glm_vec3_copy(new_pos, cam->camera_pos);

    vec3 direction;
    glm_vec3_sub(center, new_pos, direction);
    glm_vec3_normalize(direction);

    glm_vec3_copy(direction, cam->camera_front);
}
void toggle_orbit(camera* cam) {
    cam->orbit.orbiting = !cam->orbit.orbiting;
    if(!cam->orbit.orbiting) {
        cam->orbit.auto_orbit = 0;
    }
}
void toggle_auto_orbit(camera* cam) {
    cam->orbit.auto_orbit = !cam->orbit.auto_orbit;
}

int is_orbiting(const camera* cam) {
    return cam->orbit.orbiting;
}