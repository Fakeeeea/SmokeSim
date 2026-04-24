//
// Created by aaa on 20/04/2026.
//

#include "grid3d.h"
#include "../common/opengl_bindings.h"

#include <glad/glad.h>

grid3d get_grid3d(const ivec3 size) {
    grid3d out;
    glm_ivec3_copy((ivec3){size[0],size[1],size[2]}, out.size);

    out.vx_idx = out.vy_idx = out.vz_idx = 0;
    out.smoke_idx = 0;

    out.pyramids_sizes = NULL;
    out.pressure_pyramid = NULL;
    out.solid_pyramid = NULL;
    out.residual_pyramid = NULL;

    return out;
}

void gen_grid_textures3d(grid3d* grid, const float t_ambient) {
    grid->vx[0] = create_texture3((ivec3){grid->size[0]+1, grid->size[1], grid->size[2]}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);
    grid->vx[1] = create_texture3((ivec3){grid->size[0]+1, grid->size[1], grid->size[2]}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);

    grid->vy[0] = create_texture3((ivec3){grid->size[0], grid->size[1]+1, grid->size[2]}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);
    grid->vy[1] = create_texture3((ivec3){grid->size[0], grid->size[1]+1, grid->size[2]}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);

    grid->vz[0] = create_texture3((ivec3){grid->size[0], grid->size[1], grid->size[2]+1}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);
    grid->vz[1] = create_texture3((ivec3){grid->size[0], grid->size[1], grid->size[2]+1}, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);

    float init[] = {0, 0, 0, t_ambient};

    grid->smoke[0] = create_texture3(grid->size, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR, init, GL_CLAMP_TO_BORDER, init);
    grid->smoke[1] = create_texture3(grid->size, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR, init, GL_CLAMP_TO_BORDER, init);

    grid->vorticity = create_texture3(grid->size, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_BORDER, NULL);

    gen_grid_pressure_solve_textures3d(grid);
}

void gen_grid_pressure_solve_textures3d(grid3d* grid) {
    ivec3 dims = {grid->size[0], grid->size[1], grid->size[2]};
    grid->pyramids_count = 0;
    while(dims[0] >= 4 && dims[1] >= 4 && dims[2] >= 4) {
        dims[0] >>= 1;
        dims[1] >>= 1;
        dims[2] >>= 1;
        grid->pyramids_count++;
    }

    grid->pyramids_sizes = malloc(grid->pyramids_count * sizeof(vec4));
    grid->pressure_pyramid = malloc(grid->pyramids_count * sizeof(texture3));
    grid->solid_pyramid = malloc(grid->pyramids_count * sizeof(texture3));
    grid->residual_pyramid = malloc(grid->pyramids_count * sizeof(texture3));

    ivec3 current_s = {grid->size[0], grid->size[1], grid->size[2]};
    for(int i = 0; i < grid->pyramids_count; ++i) {
        grid->pyramids_sizes[i][0] = current_s[0], grid->pyramids_sizes[i][1] = current_s[1], grid->pyramids_sizes[i][2] = current_s[2], grid->pyramids_sizes[i][3] = 0;
        grid->pressure_pyramid[i] = create_texture3(current_s, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_EDGE, NULL);
        grid->solid_pyramid[i] = create_texture3(current_s, GL_RGBA16, GL_RGBA, GL_FLOAT, GL_NEAREST, NULL, GL_CLAMP_TO_EDGE, NULL);
        grid->residual_pyramid[i] = create_texture3(current_s, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, NULL, GL_CLAMP_TO_EDGE, NULL);
        current_s[0] /= 2, current_s[1] /= 2, current_s[2] /= 2;
    }
}

void free_grid3d(grid3d* grid) {
    glDeleteTextures(1, &grid->vx[0].ID);
    glDeleteTextures(1, &grid->vx[1].ID);

    glDeleteTextures(1, &grid->vy[0].ID);
    glDeleteTextures(1, &grid->vy[1].ID);

    glDeleteTextures(1, &grid->vz[0].ID);
    glDeleteTextures(1, &grid->vz[1].ID);

    glDeleteTextures(1, &grid->smoke[0].ID);
    glDeleteTextures(1, &grid->smoke[1].ID);

    glDeleteTextures(1, &grid->vorticity.ID);

    for(int i = 0; i < grid->pyramids_count; ++i) {
        if(grid->pressure_pyramid != NULL)
            glDeleteTextures(1, &grid->pressure_pyramid[i].ID);
        if(grid->solid_pyramid != NULL)
            glDeleteTextures(1, &grid->solid_pyramid[i].ID);
        if(grid->residual_pyramid != NULL)
            glDeleteTextures(1, &grid->residual_pyramid[i].ID);
    }

    if(grid->pyramids_sizes != NULL)
        free(grid->pyramids_sizes);
    if(grid->pressure_pyramid != NULL)
        free(grid->pressure_pyramid);
    if(grid->solid_pyramid != NULL)
        free(grid->solid_pyramid);
    if(grid->residual_pyramid != NULL)
        free(grid->residual_pyramid);
}

void bind_physics_buffers3d(const grid3d* grid) {

    glBindTextureUnit(VX_READ, grid->vx[grid->vx_idx].ID);
    glBindTextureUnit(VY_READ, grid->vy[grid->vy_idx].ID);
    glBindTextureUnit(VZ_READ, grid->vz[grid->vz_idx].ID);
    glBindTextureUnit(SMOKE_READ, grid->smoke[grid->smoke_idx].ID);

    glBindImageTexture(VX_WRITE, grid->vx[1-grid->vx_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(VY_WRITE, grid->vy[1-grid->vy_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(VZ_WRITE, grid->vz[1-grid->vz_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(SMOKE_WRITE, grid->smoke[1-grid->smoke_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

    glBindImageTexture(PRESSURE_MAP, grid->pressure_pyramid[0].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(SOLID_MAP, grid->solid_pyramid[0].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    glBindImageTexture(VORTICITY_MAP, grid->vorticity.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
}

void bind_multigrid_textures3d(const grid3d* grid, const int current_level) {
    int next = min(grid->pyramids_count-1, current_level+1);

    glBindTextureUnit(NEXT_PRESSURE_TEXTURE, grid->pressure_pyramid[next].ID);

    glBindImageTexture(CURRENT_PRESSURE, grid->pressure_pyramid[current_level].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(CURRENT_SOLID, grid->solid_pyramid[current_level].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    glBindImageTexture(CURRENT_RESIDUAL, grid->residual_pyramid[current_level].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(NEXT_PRESSURE, grid->pressure_pyramid[next].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(NEXT_SOLID, grid->solid_pyramid[next].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    glBindImageTexture(NEXT_RESIDUAL, grid->residual_pyramid[next].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void swap_velocity_buffers3d(grid3d* grid) {
    swap_vx_buffer3d(grid);
    swap_vy_buffer3d(grid);
    swap_vz_buffer3d(grid);
}

void swap_vx_buffer3d(grid3d* grid) {
    grid->vx_idx = 1 - grid->vx_idx;
    glBindTextureUnit(VX_READ, grid->vx[grid->vx_idx].ID);
    glBindImageTexture(VX_WRITE, grid->vx[1-grid->vx_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void swap_vy_buffer3d(grid3d* grid) {
    grid->vy_idx = 1 - grid->vy_idx;
    glBindTextureUnit(VY_READ, grid->vy[grid->vy_idx].ID);
    glBindImageTexture(VY_WRITE, grid->vy[1-grid->vy_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void swap_vz_buffer3d(grid3d* grid) {
    grid->vz_idx = 1 - grid->vz_idx;
    glBindTextureUnit(VZ_READ, grid->vz[grid->vz_idx].ID);
    glBindImageTexture(VZ_WRITE, grid->vz[1-grid->vz_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

void swap_smoke_buffer3d(grid3d* grid) {
    grid->smoke_idx = 1 - grid->smoke_idx;
    glBindTextureUnit(SMOKE_READ, grid->smoke[grid->smoke_idx].ID);
    glBindImageTexture(SMOKE_WRITE, grid->smoke[1 - grid->smoke_idx].ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
}

void clear_grid3d(const grid3d* grid, const float t_ambient) {
    glClearTexImage(grid->vx[0].ID, 0, GL_RED, GL_FLOAT, 0);
    glClearTexImage(grid->vx[1].ID, 0, GL_RED, GL_FLOAT, 0);

    glClearTexImage(grid->vy[0].ID, 0, GL_RED, GL_FLOAT, 0);
    glClearTexImage(grid->vy[1].ID, 0, GL_RED, GL_FLOAT, 0);

    glClearTexImage(grid->vz[0].ID, 0, GL_RED, GL_FLOAT, 0);
    glClearTexImage(grid->vz[1].ID, 0, GL_RED, GL_FLOAT, 0);

    float clear_smoke[4] = {0,0,0,t_ambient};

    glClearTexImage(grid->smoke[0].ID, 0, GL_RGBA, GL_FLOAT, clear_smoke);
    glClearTexImage(grid->smoke[1].ID, 0, GL_RGBA, GL_FLOAT, clear_smoke);

    glClearTexImage(grid->vorticity.ID, 0, GL_RGBA, GL_FLOAT, 0);

    for(int i = 0; i < grid->pyramids_count; ++i) {
        glClearTexImage(grid->pressure_pyramid[i].ID, 0, GL_RED, GL_FLOAT, 0);
    }
}