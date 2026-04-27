//
// Created by aaa on 08/04/2026.
//

#ifndef SMOKESIM_NK_STYLES_H
#define SMOKESIM_NK_STYLES_H

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

struct nk_style_window get_menu_window_style(struct nk_context* ctx);

struct nk_style_button get_delete_style(struct nk_context* ctx);
struct nk_style_button get_creation_style(struct nk_context* ctx);

struct nk_style_button get_menu_button_style(struct nk_context* ctx);

void apply_custom_style_nuklear(struct nk_context* ctx);

#endif //SMOKESIM_NK_STYLES_H
