//
// Created by aaa on 23/04/2026.
//

#include "gui.h"

#include "nk_helpers.h"
#include "nk_styles.h"

gui_ctx init_gui_ctx(grid* grid_data,  physics_info* p_info,  physics_info* mm_p_info,  graphics_info* g_info, main_menu_info* mm_info, struct nk_context* nk_ctx) {
    gui_ctx g_ctx = (gui_ctx) {
        .nk_ctx = nk_ctx,
        .grid_data = grid_data,
        .p_info = p_info,
        .mm_p_info = mm_p_info,
        .mm_info = mm_info,
        .g_info = g_info,
        .s_info = {0,0},
    };
    return g_ctx;
}

void draw_gui(gui_ctx* g_ctx) {
    if(is_in_main_menu(g_ctx)) {
        draw_main_menu(g_ctx);
    } else if(is_in_grid_creation(g_ctx)) {
        draw_creation_settings(g_ctx);
    } else if(are_settings_open(g_ctx)){
        draw_simulation_settings(g_ctx);
    }
}

void draw_main_menu(gui_ctx* g_ctx) {
    draw_main_menu_text(g_ctx->mm_info);
}

int is_in_main_menu(const gui_ctx* g_ctx) {
    return (g_ctx->mm_info->state == MM_MAIN_SCREEN);
}

int is_in_grid_creation(const gui_ctx* g_ctx) {
    return (g_ctx->mm_info->state == MM_OPTIONS);
}

int are_settings_open(const gui_ctx* g_ctx) {
    return (g_ctx->s_info.settings_open);
}

void draw_creation_settings(gui_ctx* g_ctx) {
    int change = 0;
    struct nk_rect size = nk_rect(0,0, (float) g_ctx->g_info->screen_size[0] * SCREEN_FOR_MM_X_PERCENTAGE, (float)g_ctx->g_info->screen_size[1]);

    if(nk_begin(g_ctx->nk_ctx, "New grid", size, 0)) {
        change += draw_grid_size_settings3d(g_ctx);
        change += draw_creation_physics_settings(g_ctx);
        draw_confirm_button(g_ctx);
        draw_discard_button(g_ctx);
        nk_end(g_ctx->nk_ctx);
    }

    if(change > 0) {
        update_physics_variables(g_ctx->p_info->physics_variables_ubo, g_ctx->grid_data, g_ctx->p_info->p_settings);
    }
}

int draw_grid_size_settings3d(gui_ctx* g_ctx) {
    int change = 0;

    draw_header(g_ctx->nk_ctx, "Grid Size");
    nk_layout_row_dynamic(g_ctx->nk_ctx, 30, 3);
    change += nk_property_int(g_ctx->nk_ctx, "X", 3, &g_ctx->grid_data->grid3d_data.size[0], 1024, 1, 1);
    change += nk_property_int(g_ctx->nk_ctx, "Y", 3, &g_ctx->grid_data->grid3d_data.size[1], 1024, 1, 1);
    change += nk_property_int(g_ctx->nk_ctx, "Z", 3, &g_ctx->grid_data->grid3d_data.size[2], 1024, 1, 1);

    return change;
}

int draw_creation_physics_settings(gui_ctx* g_ctx) {
    int change = 0;
    physics_settings* s = &g_ctx->p_info->p_settings;

    draw_header(g_ctx->nk_ctx, "Physics Settings");
    nk_layout_row_dynamic(g_ctx->nk_ctx, 30, 1);
    change += nk_property_float(g_ctx->nk_ctx, "#Time Step:", 0.0001f, &s->time_step, 0.1f, 0.001f, 0.001f);
    change += nk_property_float(g_ctx->nk_ctx, "#Density:", 0.1f, &s->density, 10.0f, 0.1f, 0.05f);
    change += nk_property_float(g_ctx->nk_ctx, "#Ambient Temp:", 0.0f, &s->t_ambient, 500.0f, 1.0f, 1.0f);
    change += nk_property_float(g_ctx->nk_ctx, "#Vorticity Confinement:", 0.0f, &s->vorticity_confinement, 10.0f, 0.1f, 0.05f);
    change += nk_property_float(g_ctx->nk_ctx, "#Temp Coefficient:", 0.0f, &s->t_temp_coeff, 10.0f, 0.01f, 0.05f);
    change += nk_property_float(g_ctx->nk_ctx, "#Weight Coefficient:", 0.0f, &s->t_weight_coeff, 10.0f, 0.01f, 0.05f);
    nk_checkbox_label(g_ctx->nk_ctx, "Grid Borders", &g_ctx->p_info->enclosed);

    return change;
}

void draw_confirm_button(gui_ctx* g_ctx) {
    if(nk_button_text(g_ctx->nk_ctx, "Confirm creation", 16)) {
        gen_grid_textures(g_ctx->grid_data, g_ctx->p_info->p_settings.t_ambient);
        p_info_upload_data(g_ctx->p_info, g_ctx->grid_data);

        if(g_ctx->p_info->enclosed) {
            init_solid_map(g_ctx->grid_data, &g_ctx->p_info->p_shaders);
        }

        g_ctx->mm_info->state = MM_CLOSED;
        g_ctx->g_info->g_info3d.g_s_settings.draw_grid_lines = 0;
        g_ctx->grid_info->created = 1;
    }
}

void draw_discard_button(gui_ctx* g_ctx) {
    g_ctx->mm_info->state = MM_MAIN_SCREEN;
    g_ctx->g_info->g_info3d.g_s_settings.draw_grid_lines = 0;
}
