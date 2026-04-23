//
// Created by aaa on 23/04/2026.
//

#ifndef SMOKESIM_GUI_H
#define SMOKESIM_GUI_H

#include <cglm/cglm.h>

#include "main_menu.h"
#include "../physics.h"
#include "../graphics.h"

typedef struct settings_info {
    int settings_open;
    int obstacles_window_open;
    int emitters_window_open;
}settings_info;

typedef struct grid_info {
    int created;
    int initialized;
}grid_info;

typedef struct gui_ctx {
    grid* grid_data;
    grid_info* grid_info;

    graphics_info* g_info;
    main_menu_info* mm_info;

    physics_info* p_info;
    physics_info* mm_p_info;

    settings_info s_info;

    struct nk_context* nk_ctx;
}gui_ctx;

gui_ctx init_gui_ctx(grid* grid_data,  physics_info* p_info, physics_info* mm_p_info, graphics_info* g_info, main_menu_info* mm_info, struct nk_context* nk_ctx);

void draw_gui(gui_ctx* g_ctx);

void draw_main_menu(gui_ctx* g_ctx);
void draw_simulation_settings(gui_ctx* g_ctx);

void draw_graphics_settings()

void draw_creation_settings(gui_ctx* g_ctx);

int draw_grid_size_settings3d(gui_ctx* g_ctx);
int draw_creation_physics_settings(gui_ctx* g_ctx);
void draw_confirm_button(gui_ctx* g_ctx);
void draw_discard_button(gui_ctx* g_ctx);

int is_in_main_menu(const gui_ctx* g_ctx);
int is_in_grid_creation(const gui_ctx* g_ctx);
int are_settings_open(const gui_ctx* g_ctx);



#endif //SMOKESIM_GUI_H
