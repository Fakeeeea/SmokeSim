//
// Created by aaa on 08/03/2026.
//


#ifndef SMOKESIM_GRID_H
#define SMOKESIM_GRID_H

#include "opengl_bindings.h"
#include "texture.h"
#include "shader.h"
#include "obstacles.h"
#include "emitters.h"
#include "../3d/grid3d.h"
#include "../2d/grid2d.h"

typedef struct grid {
    int is_2d;

    float cell_size;
    grid2d grid2d_data;
    grid3d grid3d_data;

    unsigned int multigrid_variables_ubo;
    unsigned int multigrid_constants_ubo;
}grid;


void bind_physics_buffers(const grid* grid);
void gen_grid_textures(grid *grid, float t_ambient);
void free_grid(grid *grid);

void clear_grid(const grid *grid, float t_ambient);

void update_velocities(grid* grid, const physics_shaders* shaders);
void advect_velocities(grid* grid, const physics_shaders* shaders);

void handle_emitters(grid* grid, const physics_shaders* shaders);
void update_emitters_status(const physics_shaders* shaders, unsigned int emitters_count, unsigned int type);

void advect_smoke(grid* grid, const physics_shaders* shaders);

void calculate_vorticity(const grid *grid, const physics_shaders* shaders);
void apply_vorticity(grid *grid, const physics_shaders* shaders);

void apply_buoyancy(grid* grid, const physics_shaders* shaders);

void init_solid_map(const grid* grid, const physics_shaders* shaders);

void obstacle_update_step(const physics_shaders* shaders, obstacles_info* o_info, unsigned int type);
void update_solid_map(const grid *grid, const physics_shaders* shaders);

void swap_velocity_buffers(grid* grid);
void swap_vx_buffer(grid* grid);
void swap_vy_buffer(grid* grid);
void swap_vz_buffer(grid* grid);
void swap_smoke_buffer(grid* grid);

void swap_obstacles_buffer(obstacles_info* o_info);

void multigrid_pressure_solve(grid* grid, const physics_shaders* shaders);
void v_cycle(grid* grid, const physics_shaders* shaders, int current_level);
void smooth(const grid* grid, const physics_shaders* shaders, int iterations, int current_level);
void compute_residual(const grid *grid, const physics_shaders* shaders, int current_level);
void compute_restrict(const grid *grid, const physics_shaders* shaders, int current_level);
void prolong_and_add(const grid *grid, const physics_shaders* shaders, int current_level);

void bind_multigrid_textures(const grid* grid, int current_level);

void upload_multigrid_constants(grid* grid);
void upload_multigrid_variables(grid* grid, int current_level);
void update_multigrid_variables(const grid* grid, int current_level);

void init_placeholder_grid2d(grid* grid);
void init_placeholder_grid3d(grid* grid);

void init_main_menu_grid3d(grid* mm_grid, float t_ambient);
void init_main_menu_grid2d(grid* mm_grid, float t_ambient);

void get_grid_size(const grid* grid_data, ivec3 out);
void get_pyramid_size(const grid* grid_data, int level, ivec3 out);

#endif //SMOKESIM_GRID_H
