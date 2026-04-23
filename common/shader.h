#ifndef SMOKESIM_SHADER_H
#define SMOKESIM_SHADER_H

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

typedef struct physics_shaders {
    unsigned int resolve_pressure;

    unsigned int update_vx, update_vy, update_vz;
    unsigned int advect_vx, advect_vy, advect_vz;

    unsigned int advect_smoke;

    unsigned int apply_buoyancy;

    unsigned int calculate_vorticity;
    unsigned int apply_vorticity_x, apply_vorticity_y, apply_vorticity_z;

    unsigned int handle_emitters;

    unsigned int init_solid_map;

    unsigned int obstacle_update;
    unsigned int update_solid_map;

    unsigned int compute_residual, prolong_pressure, restrict_residual, restrict_solid, smooth;

    unsigned int update_emitters_status;
}physics_shaders;

typedef struct graphics_shaders3d {
    unsigned int direction_lines;
    unsigned int grid_lines;
    unsigned int arrows;
    unsigned int smoke;
    unsigned int vorticity;
    unsigned int obstacles;
}graphics_shaders3d;

typedef struct graphics_shaders2d {
    unsigned int smoke;
}graphics_shaders2d;

unsigned int get_graphics_program(const char* vertex_path, const char* fragment_path);
unsigned int get_compute_program(const char* compute_path);
char* parse_file(const char* filename);
void free_sources(const char* vertex_source, const char* fragment_source);
void free_source(const char* compute_path);
void delete_shaders(unsigned int vertexID, unsigned int fragmentID);
void delete_shader(unsigned int computeID);

physics_shaders compile_physics_shaders();
graphics_shaders3d compile_graphics_shaders3d();
graphics_shaders2d compile_graphics_shaders2d();

#endif //SMOKESIM_SHADER_H
