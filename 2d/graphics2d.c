//
// Created by aaa on 20/04/2026.
//

#include "graphics2d.h"

static graphics_step_settings2d get_default_g_s_settings2d() {
    return (graphics_step_settings2d) {
        .draw_velocities = 0,
        .draw_vorticity = 0,
        .draw_grid_lines = 0,
        .draw_smoke = 1,
        .draw_obstacles = 1
    };
}

graphics_info2d init_graphics_info2d() {
    graphics_info2d g_info;

    g_info.g_objects = get_graphics_objects2d();
    g_info.shaders = compile_graphics_shaders2d();
    g_info.g_s_settings = get_default_g_s_settings2d();

    return g_info;
}
