//
// Created by aaa on 20/04/2026.
//

#include "physics.h"

physics_settings get_default_p_settings() {
    physics_settings p_settings = {
            .wind = {0,0,0,0},
            .time_step = 1/120.0f,
            .density = 1,
            .t_ambient = 293,
            .t_temp_coeff = 1.0f,
            .t_weight_coeff = 0.07f,
            .vorticity_confinement = 1.0f,
            ._pad0 = 0.0f,
            ._pad1 = 0.0f,
    };
    return p_settings;
}

physics_step_settings get_default_p_s_settings() {
    physics_step_settings p_s_settings = {
            .resolve_pressure = 1,
            .apply_buoyancy = 1,
            .apply_vorticity = 1,
            .advect_smoke = 1,
            .advect_velocities = 1,
            .handle_emitters = 1,
            .update_velocities = 1,
    };
    return p_s_settings;
}

physics_info init_physics_info(const physics_shaders precompiled_shaders) {
    physics_info p_info;

    p_info.paused = 0;
    p_info.enclosed = 1;

    p_info.p_s_settings = get_default_p_s_settings();
    p_info.p_settings = get_default_p_settings();

    p_info.p_shaders = precompiled_shaders;

    p_info.o_info = init_obstacles_info();
    p_info.e_info = init_emitters_info();

    return p_info;
}

void p_info_upload_data(physics_info* p_info, const grid* grid) {
    upload_obstacles(p_info->o_info.obstacles_array, p_info->o_info.obstacles_count, p_info->o_info.obstacles_ssbos);
    p_info->e_info.emitters_ssbo = upload_emitters(p_info->e_info.emitters_array, p_info->e_info.emitters_count);
    p_info->physics_variables_ubo = upload_physics_variables(grid, p_info->p_settings);
    p_info->time_ubo = upload_time(0);
}

void p_info_update_data_notime(const physics_info* p_info, const grid* grid) {
    update_obstacles(p_info->o_info.obstacles_array, p_info->o_info.obstacles_count, p_info->o_info.obstacles_ssbos);
    update_emitters(p_info->e_info.emitters_ssbo, p_info->e_info.emitters_array, p_info->e_info.emitters_count);
    update_physics_variables(p_info->physics_variables_ubo, grid, p_info->p_settings);
}

void rebind_p_info(const physics_info* p_info) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_READ, p_info->o_info.obstacles_ssbos[p_info->o_info.obstacles_idx]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_WRITE, p_info->o_info.obstacles_ssbos[1 - p_info->o_info.obstacles_idx]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, EMITTERS_SSBO, p_info->e_info.emitters_ssbo);
    glBindBufferBase(GL_UNIFORM_BUFFER, PHYSICS_VARIABLES_UBO, p_info->physics_variables_ubo);
}

void run_physics_step(grid* grid, physics_info* p_info) {
    physics_step_settings p_s_settings = p_info->p_s_settings;
    physics_shaders p_shaders = p_info->p_shaders;

    if(p_s_settings.handle_emitters) {
        update_emitters_status(&p_shaders, p_info->e_info.emitters_count, grid->is_2d);
        handle_emitters(grid, &p_shaders);
    }

    if(p_s_settings.advect_velocities) advect_velocities(grid, &p_shaders);

    if(p_s_settings.apply_buoyancy) apply_buoyancy(grid, &p_shaders);


    if(p_s_settings.apply_vorticity) {
        calculate_vorticity(grid, &p_shaders);
        apply_vorticity(grid, &p_shaders);
    }

    if(p_s_settings.resolve_pressure) multigrid_pressure_solve(grid, &p_shaders);

    bind_physics_buffers(grid);

    if(p_s_settings.update_velocities) update_velocities(grid, &p_shaders);

    if(p_s_settings.advect_smoke) advect_smoke(grid, &p_shaders);

    obstacle_update_step(&p_shaders, &p_info->o_info, grid->is_2d);
    update_solid_map(grid, &p_shaders);
}

physics_info get_mm_p_info(const physics_shaders precompiled_shaders, const grid* grid) {
    const int EMITTERS_NUM = 3;
    const int OBSTACLES_NUM = 1;

    physics_info p_info;
    p_info = init_physics_info(precompiled_shaders);

    p_info.e_info.emitters_count = EMITTERS_NUM;
    p_info.o_info.obstacles_count = OBSTACLES_NUM;

    p_info.e_info.emitters_array = malloc(sizeof(emitter) * EMITTERS_NUM);
    p_info.o_info.obstacles_array = malloc(sizeof(obstacle) * OBSTACLES_NUM);

    p_info.o_info.obstacles_array[0] = get_obstacle((vec3){(float) grid->grid3d_data.size[0] * 0.5f,
                                                           (float) grid->grid3d_data.size[1] * 0.5f,
                                                           (float) grid->grid3d_data.size[2] * 0.5f}, 3, (vec3){0,0,0}, 1);
    p_info.e_info.emitters_array[0] = get_emitter((ivec3){(int) (grid->grid3d_data.size[0] * 0.5), 2, (int)(grid->grid3d_data.size[2] * 0.5)}, 1, (ivec3){30,10,10}, 200, 0, 1);
    p_info.e_info.emitters_array[1] = get_emitter((ivec3){(int) (grid->grid3d_data.size[0] * 0.7), 2, (int)(grid->grid3d_data.size[2] * 0.5)}, 1, (ivec3){10,30,10}, 200, 0, 1);
    p_info.e_info.emitters_array[2] = get_emitter((ivec3){(int) (grid->grid3d_data.size[0] * 0.5), 2, (int)(grid->grid3d_data.size[2] * 0.7)}, 1, (ivec3){10,10,30}, 200, 0, 1);

    p_info_upload_data(&p_info, grid);

    return p_info;
}

//Move to obstacles.c & emitters.c in the future?

void free_p_info(physics_info* p_info) {
    if(p_info->o_info.obstacles_array != NULL) {
        p_info->o_info.obstacles_count = 0;
        free(p_info->o_info.obstacles_array);
    }

    if(p_info->e_info.emitters_array != NULL) {
        p_info->e_info.emitters_count = 0;
        free(p_info->e_info.emitters_array);
    }
}

unsigned int upload_physics_variables(const grid* grid, const physics_settings settings) {
    unsigned int UBO;
    glGenBuffers(1, &UBO);

    ivec4 grid_size;
    if(grid->is_2d) {
        grid_size[0] = grid->grid2d_data.size[0], grid_size[1] = grid->grid2d_data.size[1], grid_size[2] = 0, grid_size[3] = 0;
    } else {
        grid_size[0] = grid->grid3d_data.size[0], grid_size[1] = grid->grid3d_data.size[1], grid_size[2] = grid->grid3d_data.size[2], grid_size[3] = 0;
    }

    size_t size = sizeof(ivec4) + sizeof(physics_settings) + sizeof(float);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ivec4), grid_size);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(ivec4), sizeof(physics_settings), &settings);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(ivec4)+sizeof(physics_settings), sizeof(float), &grid->cell_size);

    glBindBufferBase(GL_UNIFORM_BUFFER, PHYSICS_VARIABLES_UBO, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return UBO;
}

void update_physics_variables(const unsigned int ubo, const grid* grid, const physics_settings settings) {

    ivec4 grid_size;
    if(grid->is_2d) {
        grid_size[0] = grid->grid2d_data.size[0], grid_size[1] = grid->grid2d_data.size[1], grid_size[2] = 0, grid_size[3] = 0;
    } else {
        grid_size[0] = grid->grid3d_data.size[0], grid_size[1] = grid->grid3d_data.size[1], grid_size[2] = grid->grid3d_data.size[2], grid_size[3] = 0;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ivec4), grid_size);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(ivec4), sizeof(physics_settings), &settings);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(ivec4)+sizeof(physics_settings), sizeof(float), &grid->cell_size);
}

unsigned int upload_time(const float time) {
    unsigned int UBO;
    glGenBuffers(1, &UBO);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float), &time, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, TIME_UBO, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return UBO;
}

void update_time(const unsigned int ubo, const float new_time) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &new_time);
}