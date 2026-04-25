//
// Created by aaa on 22/04/2026.
//

#ifndef SMOKESIM_GRAPHICS_OBJECTS2D_H
#define SMOKESIM_GRAPHICS_OBJECTS2D_H

#include "../common/graphics_objects.h"

enum DIRECTIONS2D {
    DIR2D_X,
    DIR2D_Y,
};

typedef struct graphics_objects2d {
    draw_object screen_rect;

    draw_object circle;

    draw_object arrow[2];
}graphics_objects2d;

static draw_object get_rect(const vec2 size);

graphics_objects2d get_graphics_objects2d();

#endif //SMOKESIM_GRAPHICS_OBJECTS2D_H
