//
// Created by aaa on 09/04/2026.
//

//TODO SIZE UPDATE CALLBACK

#include "main_menu.h"

void init_main_menu(main_menu_info* mm_info, const ivec2 screen_size) {

    mm_info->ft = init_ft();
    mm_info->mm_face = get_font_face(mm_info->ft, "../fonts/ProggyClean.ttf");
    mm_info->mm_font_atlas = get_font_atlas(mm_info->ft, mm_info->mm_face, BUTTON_TEXT_SIZE);

    mm_info->t_renderer = init_text_renderer();

    vec2 button_size = {screen_size[0] * BUTTONS_WIDTH_PERCENTAGE, screen_size[1] * BUTTONS_HEIGHT_PERCENTAGE};
    vec2 padding = {screen_size[0] * PADDING_FROM_LEFT_PERCENTAGE, screen_size[1] * PADDING_FROM_BOTTOM_PERCENTAGE};

    vec2 bottom_button_pos = {padding[0], padding[1]};
    vec2 bottom_button_max; glm_vec2_add(bottom_button_pos, button_size, bottom_button_max);
    vec2 middle_button_pos = {padding[0], padding[1]+button_size[1]};
    vec2 middle_button_max; glm_vec2_add(middle_button_pos, button_size, middle_button_max);
    vec2 top_button_pos = {padding[0], padding[1]+button_size[1]*2};
    vec2 top_button_max; glm_vec2_add(top_button_pos, button_size, top_button_max);

    mm_info->buttons = malloc(sizeof(menu_button) * MENU_BUTTONS_COUNT);
    mm_info->buttons[QUIT] = get_menu_button(mm_info, "Quit", bottom_button_pos, bottom_button_max, (vec3){0,0,0}, (vec2){30,0}, 1.2f);
    mm_info->buttons[LOAD_ID] = get_menu_button(mm_info, "Load Simulation", middle_button_pos, middle_button_max, (vec3){0,0,0}, (vec2){30, 0}, 1.2f);
    mm_info->buttons[NEW_SIM_ID] = get_menu_button(mm_info, "New Simulation", top_button_pos, top_button_max, (vec3){0,0,0}, (vec2){30, 0}, 1.2f);

    mm_info->buttons[BACK] = get_menu_button(mm_info, "Back", bottom_button_pos, bottom_button_max, (vec3){0,0,0}, (vec2){30, 0}, 1.2f);
    mm_info->buttons[GRID2D_ID] = get_menu_button(mm_info, "New 2D Simulation", middle_button_pos, middle_button_max, (vec3){0,0,0}, (vec2){30, 0}, 1.2f);
    mm_info->buttons[GRID3D_ID] = get_menu_button(mm_info, "New 3D Simulation", top_button_pos, top_button_max, (vec3){0,0,0}, (vec2){30, 0}, 1.2f);

    hide_button(mm_info, GRID2D_ID); hide_button(mm_info, GRID3D_ID); hide_button(mm_info, BACK);

    mm_info->state = MM_MAIN_SCREEN;
}

int is_hidden(const main_menu_info* mm_info, const int idx) {
    return mm_info->t_renderer.text_meshes[idx].hidden;
}

void hide_button(main_menu_info* mm_info, const int idx) {
    mm_info->t_renderer.text_meshes[idx].hidden = 1;
    glm_vec2_copy(mm_info->t_renderer.text_meshes[idx].hidden_offset, mm_info->t_renderer.text_meshes[idx].current_offset);
}

void unhide_button(main_menu_info* mm_info, const int idx) {
    mm_info->t_renderer.text_meshes[idx].hidden = 0;
}

static menu_button get_menu_button(main_menu_info* mm_info, char* text, vec2 min, vec2 max, vec3 color, vec2 hover_pad, float hover_scale) {

    float button_height = max[1] - min[1];
    float text_y_pos = min[1] + (button_height - mm_info->mm_font_atlas.tex.size[1]) / 2.0f;

    static_text_mesh t_mesh = get_static_text_mesh(&mm_info->mm_font_atlas, text, (vec2){min[0],text_y_pos}, (vec3){color[0], color[1], color[2]}, 1.0f);
    add_text(&mm_info->t_renderer, &t_mesh);

    menu_button mb;

    glm_vec2_copy(min, mb.min);
    glm_vec2_copy(max, mb.max);

    mb.text_idx = mm_info->t_renderer.text_count - 1;

    glm_vec2_copy(hover_pad, mm_info->t_renderer.text_meshes[mb.text_idx].target_offset);
    mm_info->t_renderer.text_meshes[mb.text_idx].hover_target_scale = hover_scale;
    return mb;
}

void draw_main_menu_text(main_menu_info* mm_info) {
    if(mm_info->state != MM_MAIN_SCREEN && mm_info->state != MM_SIMULATION_TYPE) return;

    draw_all_text(&mm_info->t_renderer);
}

void handle_hover(main_menu_info* mm_info, vec2 pos) {

    if(mm_info->state != MM_MAIN_SCREEN && mm_info->state != MM_SIMULATION_TYPE) return;

    const float lerp_speed = 0.1f;

    for(int i = 0; i < MENU_BUTTONS_COUNT; ++i) {

        if(is_hidden(mm_info, i)) continue;

        menu_button* current = &mm_info->buttons[i];

        float hover_target;
        vec2* offset_target;

        if(check_button_hover(current, pos)) {
            hover_target = mm_info->t_renderer.text_meshes[current->text_idx].hover_target_scale;
            offset_target = &mm_info->t_renderer.text_meshes[current->text_idx].target_offset;
        } else {
            hover_target = mm_info->t_renderer.text_meshes[current->text_idx].hover_base_scale;
            offset_target = &mm_info->t_renderer.text_meshes[current->text_idx].base_offset;
        }

        float* hover_current_val = &(mm_info->t_renderer.text_meshes[current->text_idx].hover_current_scale);
        vec2* offset_current_val = &(mm_info->t_renderer.text_meshes[current->text_idx].current_offset);

        lerp_f(*hover_current_val, hover_target, hover_current_val, lerp_speed);
        lerp_2f(*offset_current_val, *offset_target, offset_current_val, lerp_speed);
    }
}

void lerp_f(float current, float target, float* dest, float speed) {

    if(current == target) return;

    const float epsilon = 0.001f;
    float diff = target - current;
    if(fabsf(diff) > epsilon) {
        *dest += diff * speed;
    } else {
        *dest = target;
    }
}
void lerp_2f(vec2 current, vec2 target, vec2* dest, float speed) {
    const float epsilon = 0.001f;
    for(int i = 0; i < 2; i++) {
        float diff = target[i] - current[i];
        if(fabsf(diff) > epsilon) {
            (*dest)[i] += diff * speed;
        } else {
            (*dest)[i] = target[i];
        }
    }
}

int check_button_hover(menu_button* mb, vec2 pos) {
    if(pos[0] >= mb->min[0] && pos[0] <= mb->max[0] &&
    pos[1] >= mb->min[1] && pos[1] <= mb->max[1]) { return 1; }
    return 0;
}

int handle_click(main_menu_info* mm_info, vec2 pos) {
    if(mm_info->state != MM_MAIN_SCREEN && mm_info->state != MM_SIMULATION_TYPE) return -1;

    for(int i = 0; i < MENU_BUTTONS_COUNT; ++i) {

        if(is_hidden(mm_info, i)) continue;

        menu_button* current = &mm_info->buttons[i];

        if(check_button_hover(current, pos)) return i;
    }
    return -1;
}

void free_main_menu(main_menu_info* mm_info) {
    free(mm_info->buttons);
    free_text_renderer(&mm_info->t_renderer);
}

void get_viewport_data_mm(ivec2 screen_size, ivec4 out) {
    (out)[0] = screen_size[0] * SCREEN_FOR_MM_X_PERCENTAGE;
    (out)[1] = screen_size[1] * SCREEN_FOR_MM_Y_PERCENTAGE;
    (out)[2] = screen_size[0] - (out)[0];
    (out)[3] = screen_size[1] - (out)[1];
}

void to_simulation_type(main_menu_info* mm_info) {
    mm_info->state = MM_SIMULATION_TYPE;

    hide_button(mm_info, QUIT);
    hide_button(mm_info, NEW_SIM_ID);
    hide_button(mm_info, LOAD_ID);

    unhide_button(mm_info, GRID2D_ID);
    unhide_button(mm_info, GRID3D_ID);
    unhide_button(mm_info, BACK);
}

void to_main_screen(main_menu_info* mm_info) {
    mm_info->state = MM_MAIN_SCREEN;

    unhide_button(mm_info, QUIT);
    unhide_button(mm_info, NEW_SIM_ID);
    unhide_button(mm_info, LOAD_ID);

    hide_button(mm_info, GRID2D_ID);
    hide_button(mm_info, GRID3D_ID);
    hide_button(mm_info, BACK);
}