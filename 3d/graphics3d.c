//
// Created by aaa on 22/04/2026.
//

#include "graphics3d.h"

static graphics_step_settings3d get_default_g_s_settings3d() {
    return (graphics_step_settings3d) {
        .draw_velocities = 0,
        .draw_vorticity = 0,
        .draw_grid_lines = 0,
        .draw_direction = 1,
        .draw_smoke = 1,
        .draw_obstacles = 1,
    };
}

graphics_info3d init_graphics_info3d() {
    graphics_info3d g_info;

    g_info.is_orbiting = 1;
    g_info.orbit_distance = 0;

    g_info.cam = get_camera();
    g_info.g_objects = get_graphics_objects3d();
    g_info.shaders = compile_graphics_shaders3d();

    g_info.g_s_settings = get_default_g_s_settings3d();

    return g_info;
}

void draw_direction3d(const graphics_info3d* g_info) {
    glUseProgram(g_info->shaders.direction_lines);

    glBindVertexArray(g_info->g_objects.lines[DIR3D_X].VAO);
    glDrawElements(GL_LINES, g_info->g_objects.lines[DIR3D_X].index_count, GL_UNSIGNED_INT, 0);

    glBindVertexArray(g_info->g_objects.lines[DIR3D_Y].VAO);
    glDrawElements(GL_LINES, g_info->g_objects.lines[DIR3D_Y].index_count, GL_UNSIGNED_INT, 0);

    glBindVertexArray(g_info->g_objects.lines[DIR3D_Z].VAO);
    glDrawElements(GL_LINES, g_info->g_objects.lines[DIR3D_Z].index_count, GL_UNSIGNED_INT, 0);
}

void draw_velocity_arrows3d(const graphics_info3d* g_info, const ivec3 grid_size) {
    glUseProgram(g_info->shaders.arrows);

    static int direction_location = -1;
    if(direction_location == -1) direction_location = glGetUniformLocation(g_info->shaders.arrows, "direction");

    //x
    glBindVertexArray(g_info->g_objects.arrows[DIR3D_X].VAO);
    glUniform1i(direction_location, 0);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[DIR3D_X].index_count, GL_UNSIGNED_INT, 0, (grid_size[0] + 1) * (grid_size[1]) * (grid_size[2]));
    //y
    glBindVertexArray(g_info->g_objects.arrows[DIR3D_Y].VAO);
    glUniform1i(direction_location, 1);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[DIR3D_Y].index_count, GL_UNSIGNED_INT, 0, (grid_size[0]) * (grid_size[1] + 1) * (grid_size[2]));
    //z
    glBindVertexArray(g_info->g_objects.arrows[DIR3D_Z].VAO);
    glUniform1i(direction_location, 2);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[DIR3D_Z].index_count, GL_UNSIGNED_INT, 0, (grid_size[0]) * (grid_size[1]) * (grid_size[2] + 1));
}

void draw_smoke3d(const graphics_info3d* g_info) {
    glUseProgram(g_info->shaders.smoke);

    glBindVertexArray(g_info->g_objects.grid_box.VAO);
    glDrawElements(GL_TRIANGLES, g_info->g_objects.grid_box.index_count, GL_UNSIGNED_INT, 0);
}

void draw_vorticity3d(const graphics_info3d* g_info, const ivec3 grid_size) {
    glUseProgram(g_info->shaders.vorticity);

    glBindVertexArray(g_info->g_objects.arrows[DIR3D_X].VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[DIR3D_X].index_count, GL_UNSIGNED_INT, 0, (grid_size[0]) * (grid_size[1]) * (grid_size[2]));
}

void draw_obstacles3d(const graphics_info3d* g_info, unsigned int obstacles_count) {
    glUseProgram(g_info->shaders.obstacles);

    glBindVertexArray(g_info->g_objects.sphere.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.sphere.index_count, GL_UNSIGNED_INT, 0, obstacles_count);
}

void draw_grid3d(const graphics_info3d* g_info, const ivec3 grid_size) {
    glUseProgram(g_info->shaders.grid_lines);

    static int direction_location = -1;
    if(direction_location == -1) direction_location = glGetUniformLocation(g_info->shaders.grid_lines, "direction");

    //vertical
    glBindVertexArray(g_info->g_objects.lines[DIR3D_X].VAO);
    glUniform1i(direction_location, 0);
    glDrawElementsInstanced(GL_LINES, g_info->g_objects.lines[DIR3D_X].index_count, GL_UNSIGNED_INT, 0, (grid_size[2] + 1) * (grid_size[1] + 1));
    //horizontal
    glBindVertexArray(g_info->g_objects.lines[DIR3D_Y].VAO);
    glUniform1i(direction_location, 1);
    glDrawElementsInstanced(GL_LINES, g_info->g_objects.lines[DIR3D_Y].index_count, GL_UNSIGNED_INT, 0, (grid_size[0] + 1) * (grid_size[2] + 1));
    //depth
    glBindVertexArray(g_info->g_objects.lines[DIR3D_Z].VAO);
    glUniform1i(direction_location, 2);
    glDrawElementsInstanced(GL_LINES, g_info->g_objects.lines[DIR3D_Z].index_count, GL_UNSIGNED_INT, 0, (grid_size[0] + 1) * (grid_size[1] + 1));
}