//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_GRAPHICS2D_H
#define SMOKESIM_GRAPHICS2D_H

#include "../common/shader.h"
#include "graphics_objects2d.h"

typedef struct graphics_step_settings2d{
    int draw_velocities;
    int draw_vorticity;
    int draw_obstacles;
    int draw_grid_lines;
    int draw_smoke;
}graphics_step_settings2d;

typedef struct graphics_info2d {
    graphics_step_settings2d g_s_settings;
    graphics_shaders2d shaders;
    graphics_objects2d g_objects;
}graphics_info2d;

static graphics_step_settings2d get_default_g_s_settings2d();
graphics_info2d init_graphics_info2d();

void draw_step2d(const graphics_info2d* g_info);

#endif //SMOKESIM_GRAPHICS2D_H
