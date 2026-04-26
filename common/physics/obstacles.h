//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_OBSTACLES_H
#define SMOKESIM_OBSTACLES_H

#include <cglm/cglm.h>

typedef struct obstacle {
    vec4 pos_radius;
    vec4 velocities_anchored;
}obstacle;

typedef struct obstacles_info {
    int obstacles_count;
    obstacle* obstacles_array;
    unsigned int obstacles_ssbos[2];
    int obstacles_idx;
}obstacles_info;

obstacles_info init_obstacles_info();

void handle_obstacle_creation(obstacle** obstacles, int* obstacle_count, unsigned int* SSBOs);
void handle_obstacle_deletion(obstacle** obstacles, int* obstacle_count, unsigned int* SSBOs, int deleted_id);

obstacle get_obstacle(const vec3 pos, float radius, const vec3 velocities, int anchored);

void upload_obstacles(const obstacle* obstacles, int obstacle_count, unsigned int* SSBOs);
void update_obstacles(const obstacle* obstacles, int obstacle_count, const unsigned int* SSBOs);

void extract_obstacles_data(obstacle* obstacles, int obstacle_count, const unsigned int* SSBOs);

#endif //SMOKESIM_OBSTACLES_H
