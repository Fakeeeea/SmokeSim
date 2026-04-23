//
// Created by aaa on 22/03/2026.
//

#ifndef SMOKESIM_WINDOW_H
#define SMOKESIM_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "graphics.h"
#include "physics.h"
#include "main_menu.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:5287)
#pragma warning(disable:4116)
#endif

#include "../nuklear/nuklear.h"
#include "../nuklear/nuklear_glfw_gl3.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

typedef struct mouse_info {
    vec2 pos, last;
    int is_first_mouse;
}mouse_info;

typedef struct window_context {
    GLFWwindow* window;

    graphics_info* g_info;

    grid* grid;
    physics_info* p_info;

    main_menu_info* mm_info;
    physics_info* mm_p_info;

    mouse_info m_info;

    int init_grid;
    int created_grid;

    int settings_open;

    orbit_info mov_info;

    int is_obstacles_window_open;
    int is_emitters_window_open;

    struct nk_context* nk_ctx;
}window_context;

window_context ctx;

void init_glfw();
GLFWwindow* create_window();
int load_glad();

void init_window_context(GLFWwindow* window, struct nk_context* nk_ctx, grid* grid, graphics_info* g_info, physics_info* p_info, physics_info* mm_p_info, main_menu_info* mm_info);

void setup_callbacks();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input();
void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void scroll_wheel_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int draw_gui();

//main gui
void draw_graphics_settings();
void draw_graphics_step_settings();
int draw_physics_settings();
void draw_physics_step_settings();

//general info
void draw_camera_info();

//emitters gui
void draw_emitters_gui();
int draw_emitter_info(int e_num);

//obstacles gui
void draw_obstacles_gui();
int draw_obstacle_info(int o_num);

//main menu options
//TODO KEEP HERE
void init_new_grid_screen();

#endif //SMOKESIM_WINDOW_H
