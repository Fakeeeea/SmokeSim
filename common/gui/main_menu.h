//
// Created by aaa on 09/04/2026.
//

//TODO REWRITE TO MATCH OTHER FILES READABILITY
//TODO REWRITE TEXT.H & TEXT.C FOR THE SAME PURPOUSE

#ifndef SMOKESIM_MAIN_MENU_H
#define SMOKESIM_MAIN_MENU_H

#include <cglm/cglm.h>
#include "text.h"


#define BUTTON_TEXT_SIZE 60
#define BUTTONS_WIDTH_PERCENTAGE 0.3f
#define BUTTONS_HEIGHT_PERCENTAGE 0.15f
#define PADDING_FROM_LEFT_PERCENTAGE 0.1f
#define PADDING_FROM_BOTTOM_PERCENTAGE 0.2f
#define SCREEN_FOR_MM_X_PERCENTAGE 0.4f
#define SCREEN_FOR_MM_Y_PERCENTAGE 0

#define MENU_BUTTONS_COUNT 2

#define NEW_SIM_ID 0
#define LOAD_ID 1

enum main_menu_state {
    MM_CLOSED = 0,
    MM_MAIN_SCREEN = 1,
    MM_OPTIONS = 2,
};

struct main_menu_info;

typedef struct menu_button {
    int text_idx;
    vec2 min;
    vec2 max;
}menu_button;

typedef struct main_menu_info {
    FT_Library ft;
    FT_Face mm_face;

    font_atlas mm_font_atlas;
    menu_button* buttons;

    text_renderer t_renderer;

    int state;
}main_menu_info;

void init_main_menu(main_menu_info* mm_info, ivec2 screen_size);
static menu_button get_menu_button(main_menu_info* mm_info, char* text, vec2 min, vec2 max, vec3 color, vec2 hover_pad, float hover_scale);
void draw_main_menu_text(main_menu_info* mm_info);
void handle_hover(main_menu_info* mm_info, vec2 pos);
int handle_click(main_menu_info* mm_info, vec2 pos);
int check_hover(menu_button* mb, vec2 pos);
void free_main_menu(main_menu_info* mm_info);
void lerp_f(float current, float target, float* dest, float speed);
void lerp_2f(vec2 current, vec2 target, vec2* dest, float speed);

void get_viewport_data_mm(ivec2 screen_size, ivec4 out);

#endif //SMOKESIM_MAIN_MENU_H
