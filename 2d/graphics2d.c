//
// Created by aaa on 20/04/2026.
//

#include "graphics2d.h"

static graphics_step_settings2d get_default_g_s_settings2d() {
    return (graphics_step_settings2d) {
        .draw_velocities = 0,
        .draw_vorticity = 0,
        .draw_grid_lines = 0,
        .draw_smoke = 1,
        .draw_obstacles = 1
    };
}

graphics_info2d init_graphics_info2d() {
    graphics_info2d g_info;

    g_info.g_objects = get_graphics_objects2d();
    g_info.shaders = compile_graphics_shaders2d();
    g_info.g_s_settings = get_default_g_s_settings2d();

    return g_info;
}

void draw_smoke2d(const graphics_info2d* g_info) {
    glUseProgram(g_info->shaders.draw_smoke);

    glBindVertexArray(g_info->g_objects.screen_rect.VAO);
    glDrawElements(GL_TRIANGLES, g_info->g_objects.screen_rect.index_count, GL_UNSIGNED_INT, 0);
}

void draw_obstacles2d(const graphics_info2d* g_info, const unsigned int draw_obstacles) {
    glUseProgram(g_info->shaders.draw_obstacles);

    glBindVertexArray(g_info->g_objects.circle.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.circle.index_count, GL_UNSIGNED_INT, 0, draw_obstacles);
}

void draw_velocities2d(const graphics_info2d* g_info, const ivec2 grid_size) {

    static int direction_location = -1;

    if(direction_location == -1) {
        direction_location = glGetUniformLocation(g_info->shaders.draw_velocities, "direction");
    }

    glUseProgram(g_info->shaders.draw_velocities);

    glUniform1i(direction_location, 0);
    glBindVertexArray(g_info->g_objects.arrows[0].VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[0].index_count, GL_UNSIGNED_INT, 0, (grid_size[0] + 1) * grid_size[1]);

    glUniform1i(direction_location, 1);
    glBindVertexArray(g_info->g_objects.arrows[1].VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[1].index_count, GL_UNSIGNED_INT, 0, grid_size[0] * (grid_size[1] + 1));
}

void draw_vorticity2d(const graphics_info2d* g_info, const ivec2 grid_size) {
    glUseProgram(g_info->shaders.draw_vorticity);

    glBindVertexArray(g_info->g_objects.arrows[DIR2D_X].VAO);
    glDrawElementsInstanced(GL_TRIANGLES, g_info->g_objects.arrows[DIR2D_X].index_count, GL_UNSIGNED_INT, 0, (grid_size[0]) * (grid_size[1]));
}

void draw_grid2d(const graphics_info2d* g_info, const ivec2 grid_size) {
    glUseProgram(g_info->shaders.draw_grid_lines);

    static int direction_location = -1;
    if(direction_location == -1) direction_location = glGetUniformLocation(g_info->shaders.draw_grid_lines, "direction");

    glBindVertexArray(g_info->g_objects.lines[DIR2D_X].VAO);
    glUniform1i(direction_location, 0);
    glDrawElementsInstanced(GL_LINES, g_info->g_objects.lines[DIR2D_X].index_count, GL_UNSIGNED_INT, 0, grid_size[1] + 1);

    glBindVertexArray(g_info->g_objects.lines[DIR2D_Y].VAO);
    glUniform1i(direction_location, 1);
    glDrawElementsInstanced(GL_LINES, g_info->g_objects.lines[DIR2D_Y].index_count, GL_UNSIGNED_INT, 0, grid_size[0] + 1);
}
