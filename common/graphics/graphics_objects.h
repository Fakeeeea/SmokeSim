//
// Created by aaa on 22/03/2026.
//

#ifndef SMOKESIM_GRAPHICS_OBJECTS_H
#define SMOKESIM_GRAPHICS_OBJECTS_H

#include <cglm/cglm.h>

typedef struct draw_object {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int index_count;
}draw_object;

#endif //SMOKESIM_GRAPHICS_OBJECTS_H
