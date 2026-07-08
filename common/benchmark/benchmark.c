//
// Created by aaa on 31/05/2026.
//

#include <stdlib.h>
#include <time.h>

#include "benchmark.h"

#include <string.h>

#define MAX_LABEL_LEN 64
#define MAX_SAVE_FILENAME 256

const char* labels[] = {"EmittersUpdate",
                        "HandleEmitters",
                        "AdvectVelocities",
                        "ApplyBuoyancy",
                        "CalculateVorticity",
                        "ApplyVorticity",
                        "PressureSolve",
                        "UpdateVelocities",
                        "AdvectSmoke",
                        "UpdateObstacles",
                        "UpdateSolidMap",
                        "DrawDirectionLines",
                        "DrawGridLines",
                        "DrawVelocityArrows",
                        "DrawSmoke",
                        "DrawVorticity",
                        "DrawObstacles"
};

benchmark_info init_benchmark_info(const int benchmarking) {

    benchmark_info b_info;
    glGenQueries(2, b_info.query_IDs);

    b_info.svf_info.fptr_save = NULL;
    b_info.benchmarking = (benchmarking) ? 1 : 0;

    memset(&b_info.run_time, 0, sizeof(b_info.run_time));

    if(b_info.benchmarking) {
        create_save_file(&b_info);
    }

    return b_info;
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

    b_info->svf_info.fptr_save = fopen(filename, "w");

    init_save_file(b_info);
}

void init_save_file(benchmark_info* b_info) {
    //for(int i = 0; i < LABELS_COUNT; ++i) { fwrite(labels[i], sizeof(char), sizeof(labels[i]), b_info->svf_info.fptr_save); }
    for (int i = 0; i < LABELS_COUNT; ++i) { fprintf(b_info->svf_info.fptr_save,"%s%c", labels[i], (i == LABELS_COUNT-1) ? '\n' : ','); }
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

void save_current(benchmark_info* b_info, const unsigned int LABEL) {
    if(b_info->svf_info.fptr_save == NULL) create_save_file(b_info);
    if(b_info->svf_info.fptr_save == NULL) return;

    int stop_timer_available = 0;
    while (!stop_timer_available) {
        glGetQueryObjectiv(b_info->query_IDs[1],
                           GL_QUERY_RESULT_AVAILABLE,
                           &stop_timer_available);
    }

    glGetQueryObjectui64v(b_info->query_IDs[0], GL_QUERY_RESULT, &b_info->start_time);
    glGetQueryObjectui64v(b_info->query_IDs[1], GL_QUERY_RESULT, &b_info->end_time);

    b_info->run_time[LABEL] = ((double)b_info->end_time - (double)b_info->start_time) / 1000000.0;
}

void benchmark_end_frame(benchmark_info* b_info) {
    if(b_info->svf_info.fptr_save != NULL) {
        for (int i = 0; i < LABELS_COUNT; ++i) {
            fprintf(b_info->svf_info.fptr_save, "%f,", b_info->run_time[i]);
            b_info->run_time[i] = 0.0;
        }
    }
    fprintf(b_info->svf_info.fptr_save, "\n");
}

void free_benchmark_info(benchmark_info* b_info) {
    glDeleteQueries(2, b_info->query_IDs);

    if(b_info->svf_info.fptr_save)
        fclose(b_info->svf_info.fptr_save);
}