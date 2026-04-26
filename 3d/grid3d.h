//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_GRID3D_H
#define SMOKESIM_GRID3D_H

#include "../common/util/texture.h"

typedef struct grid3d {
    ivec3 size;

    texture3 vx[2];
    texture3 vy[2];
    texture3 vz[2];

    texture3 smoke[2];

    texture3 vorticity;

    texture3* pressure_pyramid;
    texture3* solid_pyramid;
    texture3* residual_pyramid;

    ivec4* pyramids_sizes;
    int pyramids_count;

    int vx_idx, vy_idx, vz_idx;
    int smoke_idx;
}grid3d;

grid3d get_grid3d(const ivec3 size);

void gen_grid_textures3d(grid3d* grid, float t_ambient);
void gen_grid_pressure_solve_textures3d(grid3d* grid);
void free_grid3d(grid3d* grid);

void bind_physics_buffers3d(const grid3d* grid);
void bind_multigrid_textures3d(const grid3d* grid, int current_level);

void swap_velocity_buffers3d(grid3d* grid);
void swap_vx_buffer3d(grid3d* grid);
void swap_vy_buffer3d(grid3d* grid);
void swap_vz_buffer3d(grid3d* grid);
void swap_smoke_buffer3d(grid3d* grid);

void clear_grid3d(const grid3d* grid, float t_ambient);


#endif //SMOKESIM_GRID3D_H
