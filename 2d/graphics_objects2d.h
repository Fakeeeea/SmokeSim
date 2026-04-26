//
// Created by aaa on 22/04/2026.
//

#ifndef SMOKESIM_GRAPHICS_OBJECTS2D_H
#define SMOKESIM_GRAPHICS_OBJECTS2D_H

#include "../common/graphics/graphics_objects.h"

enum DIRECTIONS2D {
    DIR2D_X,
    DIR2D_Y,
};

typedef struct graphics_objects2d {
    draw_object screen_rect;

    draw_object circle;

    draw_object arrows[2];

    draw_object lines[2];
}graphics_objects2d;

draw_object get_rect(const vec2 size);
draw_object get_arrow2d(const vec2 pointing_to);
draw_object get_sphere(float radius, int segment_count);
draw_object get_line2d(const vec2 start, const vec2 end);

graphics_objects2d get_graphics_objects2d();

#endif //SMOKESIM_GRAPHICS_OBJECTS2D_H
