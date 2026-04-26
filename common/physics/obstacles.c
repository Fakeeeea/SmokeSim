//
// Created by aaa on 20/04/2026.
//

#include "obstacles.h"
#include "../graphics/opengl_bindings.h"

#include <glad/glad.h>

obstacles_info init_obstacles_info() {
    return (obstacles_info) {
        .obstacles_count = 0,
        .obstacles_array = NULL,
        .obstacles_ssbos = {0,0},
        .obstacles_idx = 0,
    };
}

void handle_obstacle_creation(obstacle** obstacles, int* obstacle_count, unsigned int* SSBOs) {
    (*obstacle_count)++;
    size_t obstacle_size = *obstacle_count * sizeof(obstacle);
    *obstacles = realloc(*obstacles, obstacle_size);

    (*obstacles)[(*obstacle_count)-1] = get_obstacle((vec3){0,0,0}, 1, (vec3){0,0,0}, 1);

    size_t new_size = 16 + obstacle_size;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), obstacle_count);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), obstacle_count);

    update_obstacles(*obstacles, *obstacle_count, SSBOs);
}

void handle_obstacle_deletion(obstacle** obstacles, int* obstacle_count, unsigned int* SSBOs, const int deleted_id) {
    for(int i = deleted_id; i < (*obstacle_count) -1; ++i) {
        (*obstacles)[i] = (*obstacles)[i+1];
    }

    (*obstacle_count)--;

    size_t obstacles_size = sizeof(obstacle) * (*obstacle_count);
    *obstacles = realloc(*obstacles, obstacles_size);

    size_t new_size = obstacles_size + 16;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), obstacle_count);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), obstacle_count);

    update_obstacles(*obstacles, *obstacle_count, SSBOs);
}

obstacle get_obstacle(const vec3 pos, const float radius, const vec3 velocities, const int anchored) {
    obstacle o = {.pos_radius = {pos[0], pos[1], pos[2], radius},
                  .velocities_anchored = {velocities[0], velocities[1], velocities[2], (float) anchored}
    };
    return o;
}

void upload_obstacles(const obstacle* obstacles, const int obstacle_count, unsigned int* SSBOs) {
    unsigned int SSBO_IN, SSBO_OUT;

    size_t obstacle_size = obstacle_count * sizeof(obstacle);
    size_t size = 16 + obstacle_size;

    glGenBuffers(1, &SSBO_IN), glGenBuffers(1, &SSBO_OUT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_IN);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &obstacle_count);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, obstacle_size, obstacles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_READ, SSBO_IN);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_OUT);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &obstacle_count);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, obstacle_size, obstacles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBSTACLES_SSBO_WRITE, SSBO_OUT);

    SSBOs[0] = SSBO_IN, SSBOs[1] = SSBO_OUT;
}

void update_obstacles(const obstacle* obstacles, const int obstacle_count, const unsigned int* SSBOs) {
    size_t obstacle_size = obstacle_count * sizeof(obstacle);

    if(obstacle_count == 0) return;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, obstacle_size, obstacles);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, obstacle_size, obstacles);
}

void extract_obstacles_data(obstacle* obstacles, const int obstacle_count, const unsigned int* SSBOs) {
    if(obstacle_count == 0) return;
    size_t obstacle_size = obstacle_count * sizeof(obstacle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, obstacle_size, obstacles);
}
