//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_GRID2D_H
#define SMOKESIM_GRID2D_H

#include "../common/texture.h"

typedef struct grid2d {
    ivec2 size;

    texture2 vx[2];
    texture2 vy[2];

    texture2 smoke[2];

    texture2 vorticity;

    texture2* pressure_pyramid;
    texture2* solid_pyramid;
    texture2* residual_pyramid;

    ivec4* pyramids_sizes;
    int pyramids_count;

    int vx_idx, vy_idx;
    int smoke_idx;
}grid2d;

grid2d get_grid2d(const ivec2 size);

void gen_grid_textures2d(grid2d *grid, float t_ambient);
void gen_grid_pressure_solve_textures2d(grid2d* grid);
void free_grid2d(grid2d* grid);

void bind_physics_buffers2d(const grid2d* grid);
void bind_multigrid_textures2d(const grid2d* grid, int current_level);

void swap_velocity_buffers2d(grid2d* grid);
void swap_vx_buffer2d(grid2d* grid);
void swap_vy_buffer2d(grid2d* grid);
void swap_smoke_buffer2d(grid2d* grid);

#endif //SMOKESIM_GRID2D_H
