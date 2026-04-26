//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_OPENGL_BINDINGS_H
#define SMOKESIM_OPENGL_BINDINGS_H

enum texture_bindings {
    VX_READ = 0,
    VY_READ = 1,
    VZ_READ = 2,
    SMOKE_READ = 3,
    DEPTH_TEX = 4,
    VX_WRITE = 0,
    VY_WRITE = 1,
    VZ_WRITE = 2,
    SMOKE_WRITE = 3,
    PRESSURE_MAP = 4,
    SOLID_MAP = 5,
    VORTICITY_MAP = 6,
};

enum multigrid_bindings {
    NEXT_PRESSURE_TEXTURE = 5,
    CURRENT_PRESSURE = 0,
    CURRENT_SOLID = 1,
    CURRENT_RESIDUAL = 2,
    NEXT_PRESSURE = 3,
    NEXT_SOLID = 4,
    NEXT_RESIDUAL = 5
};

enum variables_bindings {
    GRAPHICS_VARIABLES_UBO = 0,
    PHYSICS_VARIABLES_UBO = 1,
    MULTIGRID_CONSTANTS_UBO = 2,
    MULTIGRID_VARIABLES_UBO = 3,
    EMITTERS_SSBO = 4,
    OBSTACLES_SSBO_READ = 5,
    OBSTACLES_SSBO_WRITE = 6,
    TIME_UBO = 7,
};

#endif //SMOKESIM_OPENGL_BINDINGS_H
