//
// Created by aaa on 23/04/2026.
//

#ifndef SMOKESIM_NK_HELPERS_H
#define SMOKESIM_NK_HELPERS_H

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

#include "../../nuklear/nuklear.h"
#include "../../nuklear/nuklear_glfw_gl3.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <cglm/cglm.h>

#define BUFFER_SIZE 32

void draw_color_picker(struct nk_context* nk_ctx,  const char* label, vec3 var, float mul);
void draw_header(struct nk_context* nk_ctx, const char* label);

int draw_ivec2_property_keepratio(struct nk_context* nk_ctx, const char* label, int* var, int min, int max, int step, float ratio);

int draw_vec2_property(struct nk_context* nk_ctx, const char* label, float* var, float min, float max, float step);
int draw_ivec2_property(struct nk_context* nk_ctx, const char* label, int* var, int min, int max, int step);

int draw_vec3_property(struct nk_context* nk_ctx, const char* label, float* var, float min, float max, float step);
int draw_ivec3_property(struct nk_context* nk_ctx, const char* label, int* var, int min, int max, int step);

int draw_ivec2_property_xy_id(struct nk_context* nk_ctx, const char* label, int* var, int id, int min, int max, int step);

int draw_ivec3_property_xyz_id(struct nk_context* nk_ctx, const char* label, int* var, int id, int min, int max, int step);
int draw_ivec3_property_rgb_id(struct nk_context* nk_ctx, const char* label, int* var, int id, int min, int max, int step);

int draw_vec2_property_xy_id(struct nk_context* nk_ctx, const char* label, float* var, int id, float min, float max, float step);
int draw_vec3_property_xyz_id(struct nk_context* nk_ctx, const char* label, float* var, int id, float min, float max, float step);

int draw_create_button(struct nk_context* nk_ctx, const char* label, int length);
int draw_delete_button(struct nk_context* nk_ctx, const char* label, int length);

#endif //SMOKESIM_NK_HELPERS_H
