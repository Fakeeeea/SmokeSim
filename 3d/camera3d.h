//
// Created by aaa on 08/03/2026.
//

#ifndef SMOKESIM_CAMERA3D_H
#define SMOKESIM_CAMERA3D_H

#include <cglm/cglm.h>

typedef struct orbit_info {
    int orbiting;
    int auto_orbit;
    float orbit_distance;
}orbit_info;

typedef struct camera {
    vec3 camera_pos;
    vec3 camera_front;
    vec3 camera_up;
    vec3 angle; // yaw - pitch - roll

    orbit_info orbit;
}camera;

camera get_camera();

void get_lookat(camera cam, mat4 dest);

void move_camera_z(camera* cam, float amount);
void mouse_movement(camera* cam, const vec2 offset);
void mouse_movement_orbit(camera* cam, const ivec3 grid_size, const vec2 offset);
void rotate_camera_around_center(camera* cam, const vec3 center, const float angle, const float time);

void auto_orbit(camera* cam, const ivec3 grid_size, float time);

void toggle_orbit(camera* cam);
void toggle_auto_orbit(camera* cam);

int is_orbiting(const camera* cam);

#endif //SMOKESIM_CAMERA3D_H
