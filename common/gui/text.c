//
// Created by aaa on 10/04/2026.
//

#include <string.h>

#include "../shader.h"
#include "text.h"

FT_Library init_ft() {
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        printf("Could not init FreeType library\n");
        exit(-1);
    }

    return ft;
}

FT_Face get_font_face(FT_Library ft, char* src) {
    FT_Face face;
    if(FT_New_Face(ft, src, 0, &face)) {
        printf("Failed to load font (%s)\n", src);
        exit(-1);
    }

    return face;
}

font_atlas get_font_atlas(FT_Library ft, FT_Face face, unsigned int px_size) {
    font_atlas a;

    FT_Set_Pixel_Sizes(face, 0, px_size);

    FT_GlyphSlot g = face->glyph;

    unsigned int w = 0;
    unsigned int h = 0;
    for(int i = 32; i < 128; ++i) {
        if(FT_Load_Char(face, (char)i, FT_LOAD_RENDER)) continue;

        w += g->bitmap.width;
        if(h < g->bitmap.rows) h = g->bitmap.rows;
    }

    unsigned char* pixels = calloc(w*h, 1);
    int x_offset = 0;

    for(int i = 32; i < 128; ++i) {
        if(FT_Load_Char(face, (char)i, FT_LOAD_RENDER)) continue;

        for(unsigned int row = 0; row < g->bitmap.rows; ++row) {
            for(unsigned int col = 0; col < g->bitmap.width; ++col) {
                int x = x_offset + col;
                int y = row;
                pixels[y * w + x] = g->bitmap.buffer[row * g->bitmap.width + col];
            }
        }

        a.glyphs[i].ax = (g->advance.x >> 6);
        a.glyphs[i].bw = g->bitmap.width;
        a.glyphs[i].bh = g->bitmap.rows;
        a.glyphs[i].bl = g->bitmap_left;
        a.glyphs[i].bt = g->bitmap_top;
        a.glyphs[i].tx = (float) x_offset / (float) w;
        a.glyphs[i].ty = (float) g->bitmap.rows / (float) h;

        x_offset += g->bitmap.width;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    a.tex = create_texture2_buf((ivec2){w,h}, GL_RED, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, pixels, GL_CLAMP_TO_EDGE, NULL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    free(pixels);

    return a;
}

text_renderer init_text_renderer() {
    text_renderer t;
    t.text_meshes = NULL;
    t.text_shader = get_graphics_program("../common_shaders/graphics/text/text_shader.vert",
                                         "../common_shaders/graphics/text/text_shader.frag");
    t.text_count = 0;
    return t;
}

static_text_mesh get_static_text_mesh(font_atlas* atlas, const char* text, vec2 pos, vec3 color, float scale) {
    static_text_mesh m;
    unsigned int len = strlen(text);

    size_t vertices_size = 4 * sizeof(text_vertex) * len;
    size_t indices_size = 6 * sizeof(int) * len;

    text_vertex* vertices = malloc(vertices_size);
    int* indices = malloc(indices_size);

    m.index_count = 6 * len;
    m.bound_atlas = atlas;
    m.base_offset[0] = 0, m.base_offset[1] = 0;
    m.current_offset[0] = 0, m.current_offset[1] = 0;
    m.target_offset[0] = 0, m.target_offset[1] = 0;
    m.hover_base_scale = 1.0f;
    m.hover_current_scale = 1.0f;
    m.hover_target_scale = 1.0f;
    m.hidden_offset[0] = -1000.0f, m.hidden_offset[1] = 0.0f;
    m.hidden = 0;
    glm_vec2_copy(pos, m.pivot);
    glm_vec3_copy(color, m.text_color);

    int valid_chars = 0;
    for(const char* p = text; *p; p++) {
        if(*p < 32 || *p >= 128) continue;

        glyph_info ch = atlas->glyphs[*p];

        float xpos = pos[0] + ch.bl * scale;
        float ypos = pos[1] - (ch.bh - ch.bt) * scale;

        float w = ch.bw * scale;
        float h = ch.bh * scale;

        float u_min = ch.tx;
        float v_min = 0.0f;
        float u_max = ch.tx + ((float)ch.bw / atlas->tex.size[0]);
        float v_max = ch.ty;

        int v_offset = valid_chars * 4;
        int i_offset = valid_chars * 6;

        vertices[v_offset + 0] = (text_vertex) {xpos, ypos + h, u_min, v_min};
        vertices[v_offset + 1] = (text_vertex) {xpos, ypos , u_min, v_max};
        vertices[v_offset + 2] = (text_vertex) {xpos + w, ypos, u_max, v_max};
        vertices[v_offset + 3] = (text_vertex) {xpos + w, ypos + h, u_max, v_min};

        indices[i_offset + 0] = v_offset + 0;
        indices[i_offset + 1] = v_offset + 1;
        indices[i_offset + 2] = v_offset + 2;
        indices[i_offset + 3] = v_offset + 0;
        indices[i_offset + 4] = v_offset + 2;
        indices[i_offset + 5] = v_offset + 3;

        pos[0] += ch.ax * scale;

        valid_chars++;
    }

    glGenBuffers(1, &m.VBO), glGenVertexArrays(1, &m.VAO), glGenBuffers(1, &m.EBO);
    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(text_vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(text_vertex), (void*)(sizeof(vec2)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices), free(indices);
    return m;
}

void draw_all_text(text_renderer* renderer) {

    static int color_loc = -1;
    static int offset_loc = -1;
    static int scale_loc = -1;
    static int pivot_loc = -1;

    glDisable(GL_DEPTH_TEST);
    glUseProgram(renderer->text_shader);

    if(color_loc == -1) {
        color_loc = glGetUniformLocation(renderer->text_shader, "text_color");
        offset_loc = glGetUniformLocation(renderer->text_shader, "t_offset");
        scale_loc = glGetUniformLocation(renderer->text_shader, "scale");
        pivot_loc = glGetUniformLocation(renderer->text_shader, "pivot");
    }

    for(int i = 0; i < renderer->text_count; ++i) {
        glBindVertexArray(renderer->text_meshes[i].VAO);
        glUniform1f(scale_loc, renderer->text_meshes[i].hover_current_scale);
        glUniform2f(offset_loc, renderer->text_meshes[i].current_offset[0], renderer->text_meshes[i].current_offset[1]);
        glUniform2f(pivot_loc, renderer->text_meshes[i].pivot[0], renderer->text_meshes[i].pivot[1]);
        glUniform3f(color_loc, renderer->text_meshes[i].text_color[0], renderer->text_meshes[i].text_color[1], renderer->text_meshes[i].text_color[2]);

        glBindTextureUnit(0, renderer->text_meshes[i].bound_atlas->tex.ID);
        glDrawElements(GL_TRIANGLES, renderer->text_meshes[i].index_count, GL_UNSIGNED_INT, 0);
    }
    glEnable(GL_DEPTH_TEST);
}

void add_text(text_renderer* tr, static_text_mesh* t) {
    tr->text_count++;
    tr->text_meshes = realloc(tr->text_meshes, tr->text_count * sizeof(static_text_mesh));

    tr->text_meshes[tr->text_count-1] = *t;
}

void free_text_renderer(text_renderer* tr) {
    tr->text_count = 0;
    free(tr->text_meshes);
}