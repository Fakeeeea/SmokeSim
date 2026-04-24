//
// Created by aaa on 10/04/2026.
//

#ifndef SMOKESIM_TEXT_H
#define SMOKESIM_TEXT_H

#include <cglm/cglm.h>
#include <ft2build.h>

#include "../texture.h"

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

#include FT_FREETYPE_H

typedef struct glyph_info {
    int ax;
    int bw;
    int bh;
    int bl;
    int bt;
    float tx;
    float ty;
}glyph_info;

typedef struct font_atlas {
    texture2 tex;
    glyph_info glyphs[128];
} font_atlas;

typedef struct text_vertex {
    vec2 pos;
    vec2 uv;
}text_vertex;

typedef struct static_text_mesh {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    int index_count;
    vec3 text_color;
    font_atlas* bound_atlas;
    vec2 base_offset;
    vec2 target_offset;
    vec2 current_offset;
    vec2 hidden_offset;
    int hidden;
    float hover_base_scale;
    float hover_current_scale;
    float hover_target_scale;
    vec2 pivot;
}static_text_mesh;

typedef struct text_renderer {
    int text_count;
    static_text_mesh* text_meshes;
    unsigned int text_shader;
}text_renderer;

FT_Library init_ft();
FT_Face get_font_face(FT_Library ft, char* src);

font_atlas get_font_atlas(FT_Library ft, FT_Face face, unsigned int px_size);

text_renderer init_text_renderer();
void add_text(text_renderer* tr, static_text_mesh* t);
static_text_mesh get_static_text_mesh(font_atlas* atlas, const char* text, vec2 pos, vec3 color, float scale);
void draw_all_text(text_renderer* renderer);
void free_text_renderer(text_renderer* tr);

#endif //SMOKESIM_TEXT_H
