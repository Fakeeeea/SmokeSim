//
// Created by aaa on 23/04/2026.
//

#include "nk_helpers.h"
#include "nk_styles.h"

void draw_header(struct nk_context* nk_ctx, const char* label) {
    nk_layout_row_dynamic(nk_ctx, 30, 1);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);
    nk_layout_row_dynamic(nk_ctx, 2, 1);
    nk_rule_horizontal(nk_ctx, nk_rgba(200,200,200,200), 0);
}

void draw_subheader(struct nk_context* nk_ctx, const char* label) {
    nk_layout_row_dynamic(nk_ctx, 20, 1);
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

int draw_vec3_property(struct nk_context* nk_ctx, const char* label, float* var, const float min, const float max, const float step) {

    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s X:", label);
    change += nk_property_float(nk_ctx, buf, min, &var[0], max, step, step);
    sprintf(buf, "#%s Y:", label);
    change += nk_property_float(nk_ctx, buf, min, &var[1], max, step, step);
    sprintf(buf, "#%s Z:", label);
    change += nk_property_float(nk_ctx, buf, min, &var[2], max, step, step);
    return change;
}

int draw_vec2_property(struct nk_context* nk_ctx, const char* label, float* var, const float min, const float max, const float step){
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s X:", label);
    change += nk_property_float(nk_ctx, buf, min, &var[0], max, step, step);
    sprintf(buf, "#%s Y:", label);
    change += nk_property_float(nk_ctx, buf, min, &var[1], max, step, step);
    return change;
}

int draw_ivec2_property_keepratio(struct nk_context* nk_ctx, const char* label, int* var, const int min, const int max, const int step, const float ratio) {
    char buf[BUFFER_SIZE];

    int old_x = var[0];

    int change = 0;
    sprintf(buf, "#%s X:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);

    if(var[0] != old_x) {
        var[1] = (int)((float)var[0] / ratio);
        change = 1;
    }

    int y_before_ui = var[1];

    sprintf(buf, "#%s Y:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);

    if(var[1] != y_before_ui) {
        var[0] = (int)((float)var[1] * ratio);
        change = 1;
    }
    return change;
}

int draw_ivec2_property(struct nk_context* nk_ctx, const char* label, int* var, const int min, const int max, const int step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s X:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);
    sprintf(buf, "#%s Y:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);
    return change;
}

int draw_ivec3_property(struct nk_context* nk_ctx, const char* label, int* var, const int min, const int max, const int step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s X:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);
    sprintf(buf, "#%s Y:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);
    sprintf(buf, "#%s Z:", label);
    change += nk_property_int(nk_ctx, buf, min, &var[2], max, step, (float) step);
    return change;
}

int draw_ivec2_property_xy_id(struct nk_context* nk_ctx, const char* label, int* var, const int id, const int min, const int max, const int step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s(%d) X:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);
    sprintf(buf, "#%s(%d) Y:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);
    return change;
}

int draw_ivec3_property_xyz_id(struct nk_context* nk_ctx, const char* label, ivec4 var, const int id, const int min, const int max, const int step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s(%d) X:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);
    sprintf(buf, "#%s(%d) Y:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);
    sprintf(buf, "#%s(%d) Z:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[2], max, step, (float) step);
    return change;
}

int draw_ivec3_property_rgb_id(struct nk_context* nk_ctx, const char* label, int* var, const int id, const int min, const int max, const int step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s(%d) R:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[0], max, step, (float) step);
    sprintf(buf, "#%s(%d) G:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[1], max, step, (float) step);
    sprintf(buf, "#%s(%d) B:", label, id);
    change += nk_property_int(nk_ctx, buf, min, &var[2], max, step, (float) step);
    return change;
}

int draw_vec2_property_xy_id(struct nk_context* nk_ctx, const char* label, float* var, const int id, const float min, const float max, const float step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s(%d) X:", label, id);
    change += nk_property_float(nk_ctx, buf, min, &var[0], max, step, step);
    sprintf(buf, "#%s(%d) Y:", label, id);
    change += nk_property_float(nk_ctx, buf, min, &var[1], max, step, step);;
    return change;
}

int draw_vec3_property_xyz_id(struct nk_context* nk_ctx, const char* label, float* var, const int id, const float min, const float max, const float step) {
    char buf[BUFFER_SIZE];

    int change = 0;
    sprintf(buf, "#%s(%d) X:", label, id);
    change += nk_property_float(nk_ctx, buf, min, &var[0], max, step, step);
    sprintf(buf, "#%s(%d) Y:", label, id);
    change += nk_property_float(nk_ctx, buf, min, &var[1], max, step, step);
    sprintf(buf, "#%s(%d) Z:", label, id);
    change += nk_property_float(nk_ctx, buf, min, &var[2], max, step, step);
    return change;
}

int draw_create_button(struct nk_context* nk_ctx, const char* label, const int length) {
    struct nk_style_button creation_style = get_creation_style(nk_ctx);
    return nk_button_text_styled(nk_ctx, &creation_style, label, length);
}

int draw_delete_button(struct nk_context* nk_ctx, const char* label, const int length) {
    struct nk_style_button delete_style = get_delete_style(nk_ctx);
    return nk_button_text_styled(nk_ctx, &delete_style, label, length);
}