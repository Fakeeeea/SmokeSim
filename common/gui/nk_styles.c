//
// Created by aaa on 08/04/2026.
//

#include "nk_styles.h"

struct nk_style_window get_menu_window_style(struct nk_context* ctx) {
    struct nk_style_window window_style = ctx->style.window;

    return window_style;
}

struct nk_style_button get_delete_style(struct nk_context* ctx) {
    struct nk_style_button delete_style = ctx->style.button;
    delete_style.normal = nk_style_item_color(nk_rgb(200,30,30));
    delete_style.hover = nk_style_item_color(nk_rgb(220,50,50));
    delete_style.active = nk_style_item_color(nk_rgb(180,20,20));

    delete_style.text_normal = nk_rgb(255,255,255);
    delete_style.text_hover = nk_rgb(255,255,255);
    delete_style.text_active = nk_rgb(255,255,255);

    delete_style.border_color = nk_rgb(120,0,0);
    delete_style.rounding = 3.0f;
    return delete_style;
}


struct nk_style_button get_creation_style(struct nk_context* ctx) {
    struct nk_style_button creation_style = ctx->style.button;
    creation_style.normal = nk_style_item_color(nk_rgb(30,200,30));
    creation_style.hover = nk_style_item_color(nk_rgb(50,220,50));
    creation_style.active = nk_style_item_color(nk_rgb(20,180,20));

    creation_style.text_normal = nk_rgb(255,255,255);
    creation_style.text_hover = nk_rgb(255,255,255);
    creation_style.text_active = nk_rgb(255,255,255);

    creation_style.border_color = nk_rgb(0,120,0);
    creation_style.rounding = 3.0f;
    return creation_style;
}

struct nk_style_button get_menu_button_style(struct nk_context* ctx) {
    struct nk_style_button menu_button_style = ctx->style.button;

    menu_button_style.normal.type = NK_STYLE_ITEM_COLOR;
    menu_button_style.normal.data.color = nk_rgba(0,0,0,0);
    menu_button_style.hover.type = NK_STYLE_ITEM_COLOR;
    menu_button_style.hover.data.color = nk_rgba(0,0,0,0);
    menu_button_style.active.type = NK_STYLE_ITEM_COLOR;
    menu_button_style.active.data.color = nk_rgba(0,0,0,0);

    menu_button_style.text_hover = nk_rgb(255,0,0);

    menu_button_style.border = 0.0f;
    menu_button_style.padding = nk_vec2(0,0);

    menu_button_style.text_alignment = NK_TEXT_LEFT;

    return menu_button_style;
}

void apply_custom_style_nuklear(struct nk_context* ctx) {
    struct nk_color table[NK_COLOR_COUNT];

    table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
    table[NK_COLOR_WINDOW] = nk_rgba(25, 25, 28, 240);
    table[NK_COLOR_HEADER] = nk_rgba(35, 35, 50, 255);
    table[NK_COLOR_BUTTON] = nk_rgba(40, 40, 45, 255);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgba(55, 55, 60, 255);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_TOGGLE] = nk_rgba(40, 40, 45, 255);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(55, 55, 60, 255);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(0, 255, 120, 255);
    table[NK_COLOR_SELECT] = nk_rgba(40, 40, 45, 255);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(0, 255, 120, 255);
    table[NK_COLOR_SLIDER] = nk_rgba(40, 40, 45, 255);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(100, 100, 105, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(130, 130, 135, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(0, 255, 120, 255);
    table[NK_COLOR_PROPERTY] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_EDIT] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
    table[NK_COLOR_COMBO] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_CHART] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_CHART_COLOR] = nk_rgba(0, 255, 120, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 30, 35, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(60, 60, 65, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(80, 80, 85, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(100, 100, 105, 255);
    table[NK_COLOR_TAB_HEADER] = nk_rgba(40, 40, 45, 255);

    nk_style_from_table(ctx, table);

    ctx->style.window.rounding = 0.0f;
    ctx->style.button.rounding = 0.0f;
    ctx->style.slider.rounding = 0.0f;
    ctx->style.property.rounding = 0.0f;
    ctx->style.edit.rounding = 0.0f;
    ctx->style.scrollh.rounding = 0.0f;
    ctx->style.scrollv.rounding = 0.0f;
    ctx->style.combo.rounding = 0.0f;
    ctx->style.tab.rounding = 0.0f;

    ctx->style.checkbox.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    ctx->style.checkbox.hover = nk_style_item_color(nk_rgba(255, 255, 255, 15));
    ctx->style.checkbox.active = nk_style_item_color(nk_rgba(0, 0, 0, 0));

    ctx->style.checkbox.border = 1.0f;
    ctx->style.checkbox.border_color = nk_rgba(100, 100, 105, 255);

    ctx->style.checkbox.cursor_normal = nk_style_item_color(nk_rgba(0, 255, 120, 255));
    ctx->style.checkbox.cursor_hover = nk_style_item_color(nk_rgba(50, 255, 150, 255));
}