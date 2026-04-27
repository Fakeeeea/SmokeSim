//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_PHYSICS_H
#define SMOKESIM_PHYSICS_H

#include "grid.h"

typedef struct physics_step_settings {
    int advect_smoke;
    int advect_velocities;
    int apply_vorticity;
    int apply_buoyancy;
    int resolve_pressure;
    int update_velocities;
    int handle_emitters;
}physics_step_settings;

//MANUALLY PADDED TO 48 FOR CLARITY

typedef struct physics_settings {
    vec4 wind; // 16
    float time_step; // 20
    float density; //24
    float t_ambient; //28
    float t_weight_coeff; //32
    float t_temp_coeff; //36
    float vorticity_confinement; //40
    float _pad0; //EXPLICIT PADDING FORCED BY vec4's aligned(16)
    float _pad1; //EXPLICIT PADDING FORCED BY vec4's aligned(16)
}physics_settings;

typedef struct physics_info {
    int paused;
    physics_settings p_settings;
    physics_step_settings p_s_settings;

    physics_shaders p_shaders;

    obstacles_info o_info;
    emitters_info e_info;

    unsigned int physics_variables_ubo;
    unsigned int time_ubo;

    int enclosed;
}physics_info;

physics_settings get_default_p_settings();
physics_step_settings get_default_p_s_settings();

physics_info init_physics_info(physics_shaders precompiled_shaders);
void p_info_upload_data(physics_info* p_info, const grid* grid);
void p_info_update_data_notime(const physics_info* p_info, const grid* grid);
void rebind_p_info(const physics_info* p_info);
void free_p_info(physics_info* p_info);

void run_physics_step(grid* grid, physics_info* p_info);

physics_info get_mm_p_info(physics_shaders precompiled_shaders, const grid* grid);

unsigned int upload_physics_variables(const grid* grid, physics_settings settings);
void update_physics_variables(unsigned int ubo, const grid* grid, physics_settings settings);

unsigned int upload_time(float time);
void update_time(unsigned int ubo, float new_time);

#endif //SMOKESIM_PHYSICS_H
