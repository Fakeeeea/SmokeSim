//
// Created by aaa on 07/03/2026.
//

#ifndef SMOKESIM_GRAPHICS_H
#define SMOKESIM_GRAPHICS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

#include "../2d/graphics2d.h"
#include "../3d/graphics3d.h"

#include "../common/texture.h"
#include "../common/physics.h"
#include "../common/main_menu.h"

#define INIT_SCREEN_SIZE_X 1600
#define INIT_SCREEN_SIZE_Y 900

typedef struct graphics_settings {
    vec4 ambient_color;
    vec4 objects_color;
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
}graphics_settings;

typedef struct graphics_info {
    ivec2 screen_size;
    ivec4 draw_area;

    texture2 depth_tex, scene_color_tex;
    unsigned int scene_fbo;

    graphics_info3d g_info3d;
    graphics_info2d g_info2d;

    graphics_settings g_settings;

    unsigned int graphics_variables_ubo;
}graphics_info;

graphics_info init_graphics_info();
graphics_settings get_default_g_settings();

void upload_graphics_variables(graphics_info* g_info);
void update_graphics_variables(const graphics_info* g_info);

void draw_step(grid* grid, graphics_info* info, physics_info* p_info);

void blit_framebuffer(const graphics_info* g_info);

void graphics_clear(const graphics_info* g_info);
void enable_depth_blend();
void disable_depth();

void draw_area_reset(graphics_info* g_info);
void viewport_draw_area(graphics_info* g_info);
void viewport_reset(graphics_info* g_info);

#endif //SMOKESIM_GRAPHICS_H
