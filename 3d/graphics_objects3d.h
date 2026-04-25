//
// Created by aaa on 22/04/2026.
//

#ifndef SMOKESIM_GRAPHICS_OBJECTS3D_H
#define SMOKESIM_GRAPHICS_OBJECTS3D_H

#include "../common/graphics_objects.h"
#include <cglm/cglm.h>

enum DIRECTIONS3D {
    DIR3D_X,
    DIR3D_Y,
    DIR3D_Z
};

typedef struct graphics_objects3d {
    draw_object arrows[3];
    draw_object lines[3];
    draw_object grid_box;
    draw_object sphere;
}graphics_objects3d;

static draw_object get_arrow3d(const vec3 pointing_to);
static draw_object get_line(const vec3 start, const vec3 end);
static draw_object get_box(const vec3 bottom_left_back, const vec3 top_right_front);
static draw_object get_sphere(float radius, int sector_count, int stack_count);

graphics_objects3d get_graphics_objects3d();

#endif //SMOKESIM_GRAPHICS_OBJECTS3D_H
