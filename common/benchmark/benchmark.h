//
// Created by aaa on 31/05/2026.
//

#ifndef SMOKESIM_BENCHMARK_H
#define SMOKESIM_BENCHMARK_H

#include <stdio.h>
#include <glad/glad.h>

#define LABELS_COUNT 17

enum BENCHMARK_STATE_ENUM {
    START = 0,
    END = 1
};

enum BENCHMARK_LABELS {
    EMITTERS_UPDATE = 0,
    HANDLE_EMITTERS = 1,
    ADVECT_VELOCITIES = 2,
    APPLY_BUOYANCY = 3,
    CALCULATE_VORTICITY = 4,
    APPLY_VORTICITY = 5,
    PRESSURE_SOLVE = 6,
    UPDATE_VELOCITIES = 7,
    ADVECT_SMOKE = 8,
    UPDATE_OBSTACLES = 9,
    UPDATE_SOLID_MAP = 10,
    DRAW_DIRECTION_LINES = 11,
    DRAW_GRID_LINES = 12,
    DRAW_VELOCITY_ARROWS = 13,
    DRAW_SMOKE = 14,
    DRAW_VORTICITY = 15,
    DRAW_OBSTACLES = 16
};

extern const char* labels[];

typedef struct save_file_info {
    FILE* fptr_save;
    int current_column;
}save_file_info;

typedef struct benchmark_info {
    unsigned int benchmarking;

    GLuint64 start_time, end_time;
    unsigned int query_IDs[2];

    double run_time[LABELS_COUNT];

    save_file_info svf_info;
}benchmark_info;

benchmark_info init_benchmark_info(int benchmarking);
void create_save_file(benchmark_info* b_info);
void init_save_file(benchmark_info* b_info);

void benchmark(benchmark_info* b_info, unsigned int ENUM);

void save_current(benchmark_info* b_info, unsigned int LABEL);
void benchmark_end_frame(benchmark_info* b_info);

void free_benchmark_info(benchmark_info* b_info);


#endif