//
// Created by aaa on 23/04/2026.
//

#include "nk_helpers.h"

void draw_header(struct nk_context* nk_ctx, const char* label) {
    nk_layout_row_dynamic(nk_ctx, 30, 1);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);
    nk_layout_row_dynamic(nk_ctx, 2, 1);
    nk_rule_horizontal(nk_ctx, nk_rgba(200,200,200,200), 0);
}

void draw_color_picker(struct nk_context* nk_ctx,  const char* label, vec3 var, const float mul) {
    float inv = 1.0f / mul;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);
    nk_layout_row_dynamic(nk_ctx, 100, 1);
    struct nk_colorf col = {var[0] * inv, var[1] * inv, var[2] * inv};
    col = nk_color_picker(nk_ctx, col, NK_RGB);
    var[0] = col.r * mul, var[1] = col.g * mul, var[2] = col.b * mul;
}