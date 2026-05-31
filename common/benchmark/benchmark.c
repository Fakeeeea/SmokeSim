//
// Created by aaa on 31/05/2026.
//

#include <time.h>

#include "benchmark.h"

#define MAX_SAVE_FILENAME 256

benchmark_info init_benchmark_info(int benchmarking) {

    benchmark_info b_info;
    glGenQueries(2, b_info.query_IDs);

    b_info.fptr_save = NULL;
    b_info.benchmarking = (benchmarking) ? 1 : 0;

    return b_info;
}

void benchmark(benchmark_info* b_info, unsigned int ENUM) {
    switch(ENUM) {
        case START:
            glQueryCounter(b_info->query_IDs[0], GL_TIMESTAMP);
            break;
        case END:
        default:
            glQueryCounter(b_info->query_IDs[1], GL_TIMESTAMP);
    }
}

void create_save_file(benchmark_info* b_info) {
    time_t rawtime;
    struct tm * timeinfo;

    char filename[MAX_SAVE_FILENAME];
    char time_buffer[MAX_SAVE_FILENAME];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d_%H-%M-%S", timeinfo);
    snprintf(filename, sizeof(filename), "../benchmarks/benchmark_%s.csv", time_buffer);

    b_info->fptr_save = fopen(filename, "w");

    init_save_file_all_physics(b_info);
}

void init_save_file_all_physics(benchmark_info* b_info) {
    char csv_top_row[] = "UpEmit,HandleEmit,AdvVel,ApplyBouy,CalcVort,ApplyVort,PressureSolv,UpdateVel,AdvectSmoke,UpdateObs,UpdateSolid,end\n";
    fwrite(csv_top_row, sizeof(char), sizeof(csv_top_row) - 1, b_info->fptr_save);
}

void save_current(benchmark_info* b_info) {
    if(b_info->fptr_save == NULL) create_save_file(b_info);
    if(b_info->fptr_save == NULL) return;

    int stop_timer_available = 0;
    while (!stop_timer_available) {
        glGetQueryObjectiv(b_info->query_IDs[1],
                           GL_QUERY_RESULT_AVAILABLE,
                           &stop_timer_available);
    }

    glGetQueryObjectui64v(b_info->query_IDs[0], GL_QUERY_RESULT, &b_info->start_time);
    glGetQueryObjectui64v(b_info->query_IDs[1], GL_QUERY_RESULT, &b_info->end_time);

    fprintf(b_info->fptr_save, "%f,", (double)(b_info->end_time - b_info->start_time) / 1000000.0);
}

void benchmark_end_frame(benchmark_info* b_info) {
    if(b_info->fptr_save != NULL)
        fprintf(b_info->fptr_save, "\n");
}

void free_benchmark_info(benchmark_info* b_info) {
    glDeleteQueries(2, b_info->query_IDs);

    if(b_info->fptr_save)
        fclose(b_info->fptr_save);
}