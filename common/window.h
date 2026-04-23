//
// Created by aaa on 22/03/2026.
//

#ifndef SMOKESIM_WINDOW_H
#define SMOKESIM_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "graphics.h"
#include "physics.h"
#include "gui/gui.h"

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

    mouse_info m_info;

    gui_ctx g_ctx;
}window_context;

window_context ctx;

void init_glfw();
GLFWwindow* create_window();
int load_glad();

void init_window_context(GLFWwindow* window, struct nk_context* nk_ctx, grid* grid_data, graphics_info* g_info, physics_info* p_info, physics_info* mm_p_info, main_menu_info* mm_info);

void setup_callbacks();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input();
void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void scroll_wheel_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void init_new_grid_screen();
void get_swapped_y_pos(vec2 out);
void handle_main_menu_events();

#endif //SMOKESIM_WINDOW_H
