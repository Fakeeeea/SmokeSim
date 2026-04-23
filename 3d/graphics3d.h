//
// Created by aaa on 22/04/2026.
//

#ifndef SMOKESIM_GRAPHICS3D_H
#define SMOKESIM_GRAPHICS3D_H

#include "../common/shader.h"
#include "camera3d.h"
#include "graphics_objects3d.h"

typedef struct graphics_step_settings3d{
    int draw_direction;
    int draw_velocities;
    int draw_vorticity;
    int draw_obstacles;
    int draw_grid_lines;
    int draw_smoke;
}graphics_step_settings3d;

typedef struct graphics_info3d {
    camera cam;

    int is_orbiting;
    float orbit_distance;

    graphics_step_settings3d g_s_settings;
    graphics_shaders3d shaders;
    graphics_objects3d g_objects;
}graphics_info3d;

static graphics_step_settings3d get_default_g_s_settings3d();
graphics_info3d init_graphics_info3d();

void draw_direction3d(const graphics_info3d* g_info);
void draw_velocity_arrows3d(const graphics_info3d* g_info, const ivec3 grid_size);
void draw_smoke3d(const graphics_info3d* g_info);
void draw_vorticity3d(const graphics_info3d* g_info, const ivec3 grid_size);
void draw_obstacles3d(const graphics_info3d* g_info, unsigned int obstacles_count);
void draw_grid3d(const graphics_info3d* g_info, const ivec3 grid_size);

#endif //SMOKESIM_GRAPHICS3D_H
