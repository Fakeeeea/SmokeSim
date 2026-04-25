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
        .grid_info = {0,0},
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

//Move to main_menu.c,.h
int is_in_main_menu(const gui_ctx* g_ctx) {
    return (g_ctx->mm_info->state == MM_MAIN_SCREEN || g_ctx->mm_info->state == MM_SIMULATION_TYPE);
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
        change += draw_grid_size_settings(g_ctx);
        change += draw_creation_physics_settings(g_ctx);
        draw_confirm_button(g_ctx);
        draw_discard_button(g_ctx);
        nk_end(g_ctx->nk_ctx);
    }

    if(change > 0) {
        update_physics_variables(g_ctx->p_info->physics_variables_ubo, g_ctx->grid_data, g_ctx->p_info->p_settings);
    }
}

int draw_grid_size_settings(gui_ctx* g_ctx) {
    int change = 0;
    static int keep_ratio = 1;

    draw_header(g_ctx->nk_ctx, "Grid Size");
    if(g_ctx->grid_data->is_2d) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 30, 2);
        if(keep_ratio) {
            float ratio = (float) g_ctx->g_info->screen_size[0] / (float) g_ctx->g_info->screen_size[1];
            change += draw_ivec2_property_keepratio(g_ctx->nk_ctx, "", g_ctx->grid_data->grid2d_data.size, 3, MAX_GRID_SIZE, 2, ratio);
        } else {
            change += draw_ivec2_property(g_ctx->nk_ctx, "", g_ctx->grid_data->grid2d_data.size, 3, MAX_GRID_SIZE, 2);
        }
        nk_checkbox_label(g_ctx->nk_ctx, "Lock to screen ratio", &keep_ratio);

    } else {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 30, 3);
        change += draw_ivec3_property(g_ctx->nk_ctx, "", g_ctx->grid_data->grid3d_data.size, 3, 1024, 2);
    }


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
        g_ctx->grid_info.created = 1;
        g_ctx->s_info.settings_open = 1;
    }
}

void draw_discard_button(gui_ctx* g_ctx) {
    if(nk_button_text(g_ctx->nk_ctx, "Discard", 7)) {
        g_ctx->mm_info->state = MM_MAIN_SCREEN;
        g_ctx->g_info->g_info3d.g_s_settings.draw_grid_lines = 0;
    }
}

void draw_simulation_settings(gui_ctx* g_ctx) {
    const float WIDTH = 400;
    const float HEIGHT = 600;
    if(nk_begin(g_ctx->nk_ctx, "Settings", nk_rect(0,0,WIDTH,HEIGHT),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

        draw_physics_settings(g_ctx);
        draw_physics_step_settings(g_ctx);

        if(g_ctx->grid_data->is_2d) {
            draw_graphics_settings2d(g_ctx);
            draw_graphics_step_settings2d(g_ctx);
        } else {
            draw_graphics_settings3d(g_ctx);
            draw_graphics_step_settings3d(g_ctx);
        }

        draw_checkboxes(g_ctx);

        nk_end(g_ctx->nk_ctx);
    }

    if(g_ctx->s_info.emitters_window_open) {
        draw_emitters_settings(g_ctx);
    }

    if(g_ctx->s_info.obstacles_window_open) {
        draw_obstacles_settings(g_ctx);
    }

}

void draw_physics_settings(gui_ctx *g_ctx) {
    int change = 0;
    physics_settings* p_settings = &g_ctx->p_info->p_settings;

    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Physics Constants", NK_MINIMIZED)) {

        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        change += nk_property_float(g_ctx->nk_ctx, "#Time Step:", 0.0001f, &p_settings->time_step, 0.1f, 0.001f, 0.001f);
        change += nk_property_float(g_ctx->nk_ctx, "#Density:", 0.1f, &p_settings->density, 10.0f, 0.1f, 0.05f);
        change += nk_property_float(g_ctx->nk_ctx, "#Ambient Temp:", 0.0f, &p_settings->t_ambient, 500.0f, 1.0f, 1.0f);
        change += nk_property_float(g_ctx->nk_ctx, "#Vorticity Confinement:", 0.0f, &p_settings->vorticity_confinement, 10.0f, 0.1f, 0.05f);
        change += nk_property_float(g_ctx->nk_ctx, "#Temp Coefficient:", 0.0f, &p_settings->t_temp_coeff, 10.0f, 0.01f, 0.05f);
        change += nk_property_float(g_ctx->nk_ctx, "#Weight Coefficient:", 0.0f, &p_settings->t_weight_coeff, 10.0f, 0.01f, 0.05f);

        if(!g_ctx->p_info->enclosed) {
            if(g_ctx->grid_data->is_2d) {
                change += draw_vec2_property(g_ctx->nk_ctx, "Wind", p_settings->wind, -15, 15, 1);
            } else {
                change += draw_vec3_property(g_ctx->nk_ctx, "Wind", p_settings->wind, -15, 15, 1);
            }
        }

        nk_tree_pop(g_ctx->nk_ctx);
    }

    if(change > 0) {
        update_physics_variables(g_ctx->p_info->physics_variables_ubo, g_ctx->grid_data, g_ctx->p_info->p_settings);
    }
}

void draw_graphics_settings2d(gui_ctx *g_ctx) {
    graphics_settings* g_settings = &g_ctx->g_info->g_settings;

    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Graphics Settings", NK_MINIMIZED)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        nk_property_float(g_ctx->nk_ctx, "#Smoke Density Factor:", 0.0f, &g_settings->smoke_density_factor, 200.0f, 1.0f, 1.0f);
        draw_color_picker(g_ctx->nk_ctx, "Ambient Color: ", g_settings->ambient_color, 1.0f);
        draw_color_picker(g_ctx->nk_ctx, "Object Color: ", g_settings->objects_color, 1.0f);

        nk_tree_pop(g_ctx->nk_ctx);
    }
}

void draw_graphics_settings3d(gui_ctx *g_ctx) {
    graphics_settings* g_settings = &g_ctx->g_info->g_settings;

    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Graphics Settings", NK_MINIMIZED)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        nk_property_float(g_ctx->nk_ctx, "#Smoke Density Factor:", 0.0f, &g_settings->smoke_density_factor, 200.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Shadow Density Factor:", 0.0f, &g_settings->shadow_density_factor, 200.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Ray March Step Size:", 0.001f, &g_settings->step_size, 0.2f, 0.001f, 0.001f);
        nk_property_float(g_ctx->nk_ctx, "#Ray March Shadow Step Factor:", 1.0f, &g_settings->shadow_step_factor, 20.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Ray March Shadow Steps:", 1.0f, &g_settings->shadow_steps, 20.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Z Far:", 0.001f, &g_settings->z_far, 1000.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Z Near:", 0.001f, &g_settings->z_near, 1000.0f, 1.0f, 1.0f);
        nk_property_float(g_ctx->nk_ctx, "#Fov:", glm_rad(1), &g_settings->fov, glm_rad(90), glm_rad(1), glm_rad(1));

        draw_vec3_property(g_ctx->nk_ctx, "Light Direction", g_settings->light_direction, 0.0f, 1.0f, 0.05f);

        draw_color_picker(g_ctx->nk_ctx, "Ambient Color: ", g_settings->ambient_color, 1.0f);
        draw_color_picker(g_ctx->nk_ctx, "Light Color: ", g_settings->light_color, 1.5f);
        draw_color_picker(g_ctx->nk_ctx, "Object Color: ", g_settings->objects_color, 1.0f);

        nk_tree_pop(g_ctx->nk_ctx);
    }
}

void draw_physics_step_settings(gui_ctx* g_ctx) {
    physics_step_settings* p_s_settings = &g_ctx->p_info->p_s_settings;
    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Simulation Steps", NK_MINIMIZED)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        nk_checkbox_label(g_ctx->nk_ctx, "Handle Emitters", &p_s_settings->handle_emitters);
        nk_checkbox_label(g_ctx->nk_ctx, "Apply Vorticity", &p_s_settings->apply_vorticity);
        nk_checkbox_label(g_ctx->nk_ctx, "Apply Buoyancy", &p_s_settings->apply_buoyancy);
        nk_checkbox_label(g_ctx->nk_ctx, "Resolve Pressure", &p_s_settings->resolve_pressure);
        nk_checkbox_label(g_ctx->nk_ctx, "Update Velocities", &p_s_settings->update_velocities);
        nk_checkbox_label(g_ctx->nk_ctx, "Advect Velocities", &p_s_settings->advect_velocities);
        nk_checkbox_label(g_ctx->nk_ctx, "Advect Smoke", &p_s_settings->advect_smoke);

        nk_tree_pop(g_ctx->nk_ctx);
    }
}

void draw_graphics_step_settings3d(gui_ctx* g_ctx) {
    graphics_step_settings3d* g_s_settings = &g_ctx->g_info->g_info3d.g_s_settings;
    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Graphics Steps", NK_MINIMIZED)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Grid Lines", &g_s_settings->draw_grid_lines);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Velocities", &g_s_settings->draw_velocities);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Vorticity", &g_s_settings->draw_vorticity);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Smoke", &g_s_settings->draw_smoke);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Obstacles", &g_s_settings->draw_obstacles);

        nk_tree_pop(g_ctx->nk_ctx);
    }
}

void draw_graphics_step_settings2d(gui_ctx* g_ctx) {
    graphics_step_settings2d* g_s_settings = &g_ctx->g_info->g_info2d.g_s_settings;
    if(nk_tree_push(g_ctx->nk_ctx, NK_TREE_TAB, "Graphics Steps", NK_MINIMIZED)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Smoke", &g_s_settings->draw_smoke);
        nk_checkbox_label(g_ctx->nk_ctx, "Draw Obstacles", &g_s_settings->draw_obstacles);

        nk_tree_pop(g_ctx->nk_ctx);
    }
}

void draw_checkboxes(gui_ctx* g_ctx) {
    nk_checkbox_label(g_ctx->nk_ctx, "Emitters GUI", &g_ctx->s_info.emitters_window_open);
    nk_checkbox_label(g_ctx->nk_ctx, "Obstacles GUI", &g_ctx->s_info.obstacles_window_open);
    nk_checkbox_label(g_ctx->nk_ctx, "Pause Simulation", &g_ctx->p_info->paused);
    if(nk_button_text(g_ctx->nk_ctx, "Clear grid", 10)) {
        clear_grid(g_ctx->grid_data, g_ctx->p_info->p_settings.t_ambient);
    }
}

//TODO Add width / height to DEFINES to remove magic numbers for obstacles, main settings and emitters gui
void draw_emitters_settings(gui_ctx* g_ctx) {

    int change = 0;
    emitters_info* e_info_ptr = &g_ctx->p_info->e_info;

    if(nk_begin(g_ctx->nk_ctx, "Emitters Settings", nk_rect(400, 0, 200, 300),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

        for(int i = 0; i < e_info_ptr->emitters_count; ++i) {
            int current_change = draw_emitter_info(g_ctx, i);
            if(current_change == -1) {
                handle_emitter_creation(e_info_ptr->emitters_ssbo, &e_info_ptr->emitters_array, &e_info_ptr->emitters_count);
            } else {
                change += current_change;
            }
        }

        if(change > 0) {
            update_emitters(e_info_ptr->emitters_ssbo, e_info_ptr->emitters_array, e_info_ptr->emitters_count);
        }

        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        if(draw_create_button(g_ctx->nk_ctx, "Create new emitter", 18)) {
            handle_emitter_creation(e_info_ptr->emitters_ssbo, &e_info_ptr->emitters_array, &e_info_ptr->emitters_count);
        }

    }
    nk_end(g_ctx->nk_ctx);
}

//TODO Add width / height to DEFINES to remove magic numbers for obstacles, main settings and emitters gui
void draw_obstacles_settings(gui_ctx* g_ctx) {

    int change = 0;
    static int sync_continuously = 0;
    obstacles_info* o_info_ptr = &g_ctx->p_info->o_info;

    if(nk_begin(g_ctx->nk_ctx, "Obstacles Settings", nk_rect(400, 300, 200, 300),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        if((nk_button_text(g_ctx->nk_ctx, "Sync GPU - CPU data", 19) || sync_continuously) && o_info_ptr->obstacles_count != 0) {
            extract_obstacles_data(o_info_ptr->obstacles_array, o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
        }

        nk_checkbox_label(g_ctx->nk_ctx, "Sync data continuously", &sync_continuously);

        for(int i = 0; i < o_info_ptr->obstacles_count; ++i) {
            int current_change = draw_obstacle_info(g_ctx, i);
            if(current_change == -1) {
                handle_obstacle_deletion(&o_info_ptr->obstacles_array, &o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos, i);
            } else {
                change += current_change;
            }
        }

        if(change > 0) {
            update_obstacles(o_info_ptr->obstacles_array, o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
        }

        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);
        if(draw_create_button(g_ctx->nk_ctx, "Create new obstacle", 19)) {
            handle_obstacle_creation(&o_info_ptr->obstacles_array, &o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
        }
    }
    nk_end(g_ctx->nk_ctx);
}

int draw_emitter_info(gui_ctx* g_ctx, int index) {
    int change = 0;
    char buf[BUFFER_SIZE];
    sprintf(buf, "Emitter %i", index);

    emitter* e = &(g_ctx->p_info->e_info.emitters_array[index]);

    if(nk_tree_push_id(g_ctx->nk_ctx, NK_TREE_TAB, buf, NK_MINIMIZED, index)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);

        nk_label(g_ctx->nk_ctx, "Position:", NK_TEXT_LEFT);
        if(g_ctx->grid_data->is_2d) {
            change += draw_ivec2_property_xy_id(g_ctx->nk_ctx, "", e->pos_radius, index, -9999, 9999, 1);
        } else {
            change += draw_ivec3_property_xyz_id(g_ctx->nk_ctx, "", e->pos_radius, index, -9999, 9999, 1);
        }

        change += nk_property_int(g_ctx->nk_ctx, "#Radius", 1, &e->pos_radius[3], 9999, 1, 1);

        nk_label(g_ctx->nk_ctx, "Densities:", NK_TEXT_LEFT);
        change += draw_ivec3_property_rgb_id(g_ctx->nk_ctx, "", e->density_temp, index, 0, 1000000, 1);
        change += nk_property_int(g_ctx->nk_ctx, "#Temperature", -9999, &e->density_temp[3], 1000000, 1, 1);
        change += nk_property_int(g_ctx->nk_ctx, "#Delay between emits", 0, &e->emitter_settings[0], 100000, 1, 1);
        change += nk_checkbox_label(g_ctx->nk_ctx, "#Falloff?", &e->emitter_settings[3]);

        if(draw_delete_button(g_ctx->nk_ctx, "Delete Emitter", 14)) {
            change = -1;
        }

        nk_tree_pop(g_ctx->nk_ctx);
    }
    return change;
}

int draw_obstacle_info(gui_ctx* g_ctx, int index) {
    int change = 0;
    char buf[BUFFER_SIZE];
    sprintf(buf, "Obstacle %i", index);

    obstacle* o = &(g_ctx->p_info->o_info.obstacles_array[index]);
    int anchored_val = (int) o->velocities_anchored[3];

    if(nk_tree_push_id(g_ctx->nk_ctx, NK_TREE_TAB, buf, NK_MINIMIZED, index)) {
        nk_layout_row_dynamic(g_ctx->nk_ctx, 15, 1);

        nk_label(g_ctx->nk_ctx, "Position:", NK_TEXT_LEFT);
        if(g_ctx->grid_data->is_2d) {
            change += draw_vec2_property_xy_id(g_ctx->nk_ctx, "", o->pos_radius, index, -9999, 9999, 1);
        } else {
            change += draw_vec3_property_xyz_id(g_ctx->nk_ctx, "", o->pos_radius, index, -9999, 9999, 1);
        }
        change += nk_property_float(g_ctx->nk_ctx, "Radius", 1, &o->pos_radius[3], 9999, 1, 1);

        nk_label(g_ctx->nk_ctx, "Velocities:", NK_TEXT_LEFT);
        if(g_ctx->grid_data->is_2d) {
            change += draw_vec2_property_xy_id(g_ctx->nk_ctx, "", o->velocities_anchored, index, 0, 100000, 1);
        } else {
            change += draw_vec3_property_xyz_id(g_ctx->nk_ctx, "", o->velocities_anchored, index, 0, 100000, 1);
        }
        change += nk_checkbox_label(g_ctx->nk_ctx, "Anchored", &anchored_val);

        if(draw_delete_button(g_ctx->nk_ctx, "Delete Obstacle", 15)) {
            change = -1;
        }

        nk_tree_pop(g_ctx->nk_ctx);
    }

    o->velocities_anchored[3] = anchored_val;
    return change;
}

void toggle_settings(gui_ctx* g_ctx) {
    g_ctx->s_info.settings_open = !g_ctx->s_info.settings_open;
}

void toggle_paused(gui_ctx* g_ctx) {
    g_ctx->p_info->paused = !g_ctx->p_info->paused;
}