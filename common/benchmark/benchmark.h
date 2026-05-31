//
// Created by aaa on 31/05/2026.
//

#ifndef SMOKESIM_BENCHMARK_H
#define SMOKESIM_BENCHMARK_H

#include <stdio.h>
#include <glad/glad.h>

enum BENCHMARK_ENUM {
    START = 0,
    END = 1
};

typedef struct benchmark_info {
    unsigned int benchmarking;

    GLuint64 start_time, end_time;
    unsigned int query_IDs[2];
    FILE* fptr_save;
}benchmark_info;

benchmark_info init_benchmark_info(int benchmarking);

void benchmark(benchmark_info* b_info, unsigned int ENUM);

void create_save_file(benchmark_info* b_info);
void init_save_file_all_physics(benchmark_info* b_info);
void save_current(benchmark_info* b_info);
void benchmark_end_frame(benchmark_info* b_info);

void free_benchmark_info(benchmark_info* b_info);


#endif