//
// Created by aaa on 08/03/2026.
//

#include "grid.h"
#include <GLFW/glfw3.h>


void free_grid(grid* grid) {
    if(grid->is_2d) {
        free_grid2d(&grid->grid2d_data);
    } else {
        free_grid3d(&grid->grid3d_data);
    }
}

void bind_physics_buffers(const grid* grid) {
    if(grid->is_2d) {
        bind_physics_buffers2d(&grid->grid2d_data);
    } else {
        bind_physics_buffers3d(&grid->grid3d_data);
    }
}

void gen_grid_textures(grid *grid, const float t_ambient) {
    if(grid->is_2d) {
        gen_grid_textures2d(&grid->grid2d_data, t_ambient);
    } else {
        gen_grid_textures3d(&grid->grid3d_data, t_ambient);
    }

    upload_multigrid_constants(grid);
    upload_multigrid_variables(grid, 0);
}

void update_velocities(grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.update_vx : shaders->shaders3d.update_vx);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 1 + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.update_vy : shaders->shaders3d.update_vy);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 1 + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    if(!grid->is_2d) {
        glUseProgram(shaders->shaders3d.update_vz);
        glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 1 + 7) / 8));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    swap_velocity_buffers(grid);
}

void advect_velocities(grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.advect_vx : shaders->shaders3d.advect_vx);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 1 + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.advect_vy : shaders->shaders3d.advect_vy);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 1 + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    if(!grid->is_2d) {
        glUseProgram(shaders->shaders3d.advect_vz);
        glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 1 + 7) / 8));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    swap_velocity_buffers(grid);
}

void handle_emitters(grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.handle_emitters : shaders->shaders3d.handle_emitters);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    swap_smoke_buffer(grid);
}

void update_emitters_status(const physics_shaders* shaders, const unsigned int emitters_count, const unsigned int type) {
    glUseProgram((type) ? shaders->shaders2d.update_emitters_status : shaders->shaders3d.update_emitters_status);
    glDispatchCompute((int)ceilf((float)(emitters_count + 127) / 128), 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void advect_smoke(grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.advect_smoke : shaders->shaders3d.advect_smoke);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    swap_smoke_buffer(grid);
}

void apply_buoyancy(grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.apply_buoyancy : shaders->shaders3d.apply_buoyancy);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 1 + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    swap_vy_buffer(grid);
}

void calculate_vorticity(const grid *grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.calculate_vorticity : shaders->shaders3d.calculate_vorticity);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void apply_vorticity(grid *grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.apply_vorticity_x : shaders->shaders3d.apply_vorticity_x);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 1 + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.apply_vorticity_y : shaders->shaders3d.apply_vorticity_y);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 1 + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    if(!grid->is_2d) {
        glUseProgram(shaders->shaders3d.apply_vorticity_z);
        glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 1 + 7) / 8));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    swap_velocity_buffers(grid);
}

void init_solid_map(const grid* grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.init_solid_map : shaders->shaders3d.init_solid_map);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void obstacle_update_step(const physics_shaders* shaders, obstacles_info* o_info, const unsigned int type) {
    glUseProgram((type) ? shaders->shaders2d.obstacle_update : shaders->shaders3d.obstacle_update);
    glDispatchCompute((int)ceilf((float)(o_info->obstacles_count + 127) / 128), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    swap_obstacles_buffer(o_info);
}

void update_solid_map(const grid *grid, const physics_shaders* shaders) {
    ivec3 grid_size;
    get_grid_size(grid, grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.update_solid_map : shaders->shaders3d.update_solid_map);
    glDispatchCompute((int)ceilf((float)(grid_size[0] + 7) / 8), (int)ceilf((float)(grid_size[1] + 7) / 8), (int)ceilf((float)(grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void multigrid_pressure_solve(grid* grid, const physics_shaders* shaders) {
    v_cycle(grid, shaders, 0);
}

void v_cycle(grid* grid, const physics_shaders* shaders, int current_level) {

    const int LAST_SMOOTH_ITERATIONS = 80;
    const int SMOOTH_ITERATIONS = 16;

    int pyramids_count = grid->is_2d ? grid->grid2d_data.pyramids_count : grid->grid3d_data.pyramids_count;

    update_multigrid_variables(grid, current_level);
    bind_multigrid_textures(grid, current_level);

    if(current_level == pyramids_count - 1) {
        smooth(grid, shaders, LAST_SMOOTH_ITERATIONS, current_level);
        return;
    }

    smooth(grid, shaders, SMOOTH_ITERATIONS, current_level);
    compute_residual(grid, shaders, current_level);
    compute_restrict(grid, shaders, current_level);

    if(grid->is_2d) {
        glClearTexImage(grid->grid2d_data.pressure_pyramid[current_level+1].ID, 0, GL_RED, GL_FLOAT, (void*)0);
    } else {
        glClearTexImage(grid->grid3d_data.pressure_pyramid[current_level+1].ID, 0, GL_RED, GL_FLOAT, (void*)0);
    }

    v_cycle(grid, shaders, current_level+1);

    update_multigrid_variables(grid, current_level);
    bind_multigrid_textures(grid, current_level);

    prolong_and_add(grid, shaders, current_level);
    smooth(grid, shaders, SMOOTH_ITERATIONS, current_level);
}

void smooth(const grid* grid, const physics_shaders* shaders, const int iterations, const int current_level) {
    static int parity_location2d = -1;
    static int parity_location3d = -1;

    if(parity_location2d == -1 || parity_location3d == -1) {
        parity_location2d = glGetUniformLocation(shaders->shaders2d.smooth, "parity");
        parity_location3d = glGetUniformLocation(shaders->shaders3d.smooth, "parity");
    }

    glUseProgram((grid->is_2d) ? shaders->shaders2d.smooth : shaders->shaders3d.smooth);

    ivec3 current_grid_size;
    get_pyramid_size(grid, current_level, current_grid_size);

    int parity_location = (grid->is_2d) ? parity_location2d : parity_location3d;

    int parity = 0;
    for(int i = 0; i < iterations * 2; ++i) {
        glUniform1i(parity_location, parity);
        glDispatchCompute((int)ceilf((float)(current_grid_size[0] + 7) / 8), (int)ceilf((float)(current_grid_size[1] + 7) / 8), (int)ceilf((float)(current_grid_size[2] + 7) / 8));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        parity = 1 - parity;
    }
}
void compute_residual(const grid *grid, const physics_shaders* shaders, const int current_level) {
    ivec3 current_grid_size;
    get_pyramid_size(grid, current_level, current_grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.compute_residual : shaders->shaders3d.compute_residual);
    glDispatchCompute((int)ceilf((float)(current_grid_size[0] + 7) / 8), (int)ceilf((float)(current_grid_size[1] + 7) / 8), (int)ceilf((float)(current_grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}
void compute_restrict(const grid *grid, const physics_shaders* shaders, const int current_level) {

    int target_level = current_level + 1;

    ivec3 target_grid_size;
    get_pyramid_size(grid, target_level, target_grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.restrict_residual : shaders->shaders3d.restrict_residual);
    glDispatchCompute((int)ceilf((float)(target_grid_size[0] + 7) / 8), (int)ceilf((float)(target_grid_size[1] + 7) / 8), (int)ceilf((float)(target_grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.restrict_solid : shaders->shaders3d.restrict_solid);
    glDispatchCompute((int)ceilf((float)(target_grid_size[0] + 7) / 8), (int)ceilf((float)(target_grid_size[1] + 7) / 8), (int)ceilf((float)(target_grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void prolong_and_add(const grid *grid, const physics_shaders* shaders, const int current_level) {
    ivec3 current_grid_size;
    get_pyramid_size(grid, current_level, current_grid_size);

    glUseProgram((grid->is_2d) ? shaders->shaders2d.prolong_pressure : shaders->shaders3d.prolong_pressure);
    glDispatchCompute((int)ceilf((float)(current_grid_size[0] + 7) / 8), (int)ceilf((float)(current_grid_size[1] + 7) / 8), (int)ceilf((float)(current_grid_size[2] + 7) / 8));
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void upload_multigrid_constants(grid* grid) {
    glGenBuffers(1, &grid->multigrid_constants_ubo);

    ivec4 sizes[MAX_GRID_SIZE_ARRAY];

    int pyramids_count = grid->is_2d ? grid->grid2d_data.pyramids_count : grid->grid3d_data.pyramids_count;

    for(int i = 0; i < pyramids_count; ++i) {
        if(grid->is_2d) {
            sizes[i][0] = grid->grid2d_data.pyramids_sizes[i][0];
            sizes[i][1] = grid->grid2d_data.pyramids_sizes[i][1];
            sizes[i][2] = grid->grid2d_data.pyramids_sizes[i][2];
            sizes[i][3] = grid->grid2d_data.pyramids_sizes[i][3];
        } else {
            sizes[i][0] = grid->grid3d_data.pyramids_sizes[i][0];
            sizes[i][1] = grid->grid3d_data.pyramids_sizes[i][1];
            sizes[i][2] = grid->grid3d_data.pyramids_sizes[i][2];
            sizes[i][3] = grid->grid3d_data.pyramids_sizes[i][3];
        }

    }

    glBindBuffer(GL_UNIFORM_BUFFER, grid->multigrid_constants_ubo);
    glBufferData(GL_UNIFORM_BUFFER, MAX_GRID_SIZE_ARRAY*sizeof(ivec4)+sizeof(int), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, MAX_GRID_SIZE_ARRAY*sizeof(ivec4), sizes);
    glBufferSubData(GL_UNIFORM_BUFFER, MAX_GRID_SIZE_ARRAY*sizeof(ivec4), sizeof(int), &pyramids_count);

    glBindBufferBase(GL_UNIFORM_BUFFER, MULTIGRID_CONSTANTS_UBO, grid->multigrid_constants_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void upload_multigrid_variables(grid* grid, const int current_level) {
    glGenBuffers(1, &grid->multigrid_variables_ubo);

    float current_cell_size = grid->cell_size * (float) (1 << current_level);

    glBindBuffer(GL_UNIFORM_BUFFER, grid->multigrid_variables_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 16, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int), &current_level);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int), sizeof(float), &current_cell_size);

    glBindBufferBase(GL_UNIFORM_BUFFER, MULTIGRID_VARIABLES_UBO, grid->multigrid_variables_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void update_multigrid_variables(const grid* grid, const int current_level) {

    float current_cell_size = grid->cell_size * (float) (1 << current_level);

    glBindBuffer(GL_UNIFORM_BUFFER, grid->multigrid_variables_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int), &current_level);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int), sizeof(float), &current_cell_size);
}

void bind_multigrid_textures(const grid* grid, const int current_level) {

    if(grid->is_2d) {
        bind_multigrid_textures2d(&grid->grid2d_data, current_level);
    } else {
        bind_multigrid_textures3d(&grid->grid3d_data, current_level);
    }
}

void init_placeholder_grid3d(grid* grid) {
    const float PLACEHOLDER_CELL_SIZE = 0.1f;
    const int PLACEHOLDER_RESOLUTIONX = 64;
    const int PLACEHOLDER_RESOLUTIONY = 64;
    const int PLACEHOLDER_RESOLUTIONZ = 64;

    grid->is_2d = 0;
    grid->grid3d_data = get_grid3d((ivec3){PLACEHOLDER_RESOLUTIONX, PLACEHOLDER_RESOLUTIONY, PLACEHOLDER_RESOLUTIONZ});
    grid->cell_size = PLACEHOLDER_CELL_SIZE;
}

void init_placeholder_grid2d(grid* grid) {
    const float PLACEHOLDER_CELL_SIZE = 0.1f;
    const int PLACEHOLDER_RESOLUTIONX = 400;
    const int PLACEHOLDER_RESOLUTIONY = 225;

    grid->is_2d = 1;
    grid->grid2d_data = get_grid2d((ivec2){PLACEHOLDER_RESOLUTIONX, PLACEHOLDER_RESOLUTIONY});
    grid->cell_size = PLACEHOLDER_CELL_SIZE;
}

void init_main_menu_grid2d(grid* mm_grid, float t_ambient) {
    init_placeholder_grid2d(mm_grid);
    gen_grid_textures(mm_grid, t_ambient);
}

void init_main_menu_grid3d(grid* mm_grid, float t_ambient) {
    init_placeholder_grid3d(mm_grid);
    gen_grid_textures(mm_grid, t_ambient);
}

void get_grid_size(const grid* grid_data, ivec3 out) {
    out[0] = grid_data->is_2d ? grid_data->grid2d_data.size[0] : grid_data->grid3d_data.size[0];
    out[1] = grid_data->is_2d ? grid_data->grid2d_data.size[1] : grid_data->grid3d_data.size[1];
    out[2] = grid_data->is_2d ? 1 : grid_data->grid3d_data.size[2];
}

void get_pyramid_size(const grid* grid_data, int level, ivec3 out) {
    out[0] = grid_data->is_2d ? grid_data->grid2d_data.pyramids_sizes[level][0] : grid_data->grid3d_data.pyramids_sizes[level][0];
    out[1] = grid_data->is_2d ? grid_data->grid2d_data.pyramids_sizes[level][1] : grid_data->grid3d_data.pyramids_sizes[level][1];
    out[2] = grid_data->is_2d ? 1 : grid_data->grid3d_data.pyramids_sizes[level][2];
}

void swap_velocity_buffers(grid* grid) {
    if(grid->is_2d) {
        swap_velocity_buffers2d(&grid->grid2d_data);
    } else {
        swap_velocity_buffers3d(&grid->grid3d_data);
    }
}

void swap_vx_buffer(grid* grid) {
    if(grid->is_2d) {
        swap_vx_buffer2d(&grid->grid2d_data);
    } else {
        swap_vx_buffer3d(&grid->grid3d_data);
    }
}

void swap_vy_buffer(grid* grid) {
    if(grid->is_2d) {
        swap_vy_buffer2d(&grid->grid2d_data);
    } else {
        swap_vy_buffer3d(&grid->grid3d_data);
    }
}

void swap_vz_buffer(grid* grid) {
    swap_vz_buffer3d(&grid->grid3d_data);
}

void swap_smoke_buffer(grid* grid) {
    if(grid->is_2d) {
        swap_smoke_buffer2d(&grid->grid2d_data);
    } else {
        swap_smoke_buffer3d(&grid->grid3d_data);
    }
}

void swap_obstacles_buffer(obstacles_info* o_info) {
    o_info->obstacles_idx = 1 - o_info->obstacles_idx;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_READ, o_info->obstacles_ssbos[o_info->obstacles_idx]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_WRITE, o_info->obstacles_ssbos[1-o_info->obstacles_idx]);
}

void clear_grid(const grid *grid, const float t_ambient) {
    if(grid->is_2d) {
        clear_grid2d(&grid->grid2d_data, t_ambient);
    } else {
        clear_grid3d(&grid->grid3d_data, t_ambient);
    }
}