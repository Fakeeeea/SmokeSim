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