//
// Created by aaa on 22/04/2026.
//

#ifndef SMOKESIM_GRAPHICS_OBJECTS3D_H
#define SMOKESIM_GRAPHICS_OBJECTS3D_H

#include "../common/graphics/graphics_objects.h"
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

graphics_objects3d get_graphics_objects3d();

#endif //SMOKESIM_GRAPHICS_OBJECTS3D_H
