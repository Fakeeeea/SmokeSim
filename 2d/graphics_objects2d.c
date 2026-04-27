//
// Created by aaa on 22/04/2026.
//

#include "graphics_objects2d.h"

#include <glad/glad.h>

static draw_object get_arrow2d(const vec2 pointing_to) {
    draw_object arrow;

    float vertices[] = {
            //TAIL
            -0.05f, 0.0f,
            0.05f, 0.0f,
            -0.05f, 0.6f,
            0.05f, 0.6f,

            //HEAD
            -0.2f, 0.6f,
            0.2f, 0.6f,
            0.0f, 0.8f,
    };

    unsigned int indices[] = {
            //TAIL
            0,1,2,
            1,3,2,

            //HEAD
            4,5,6
    };

    vec2 dir;
    glm_vec2_copy((float*)pointing_to, dir); glm_vec2_normalize(dir);

    float cos_tetha = dir[1];
    float sin_tetha = -dir[0];

    const int POINTS = 7;
    for(int i = 0; i < POINTS * 2; i+=2) {

        float px = vertices[i];
        float py = vertices[i+1];

        vertices[i] = px * cos_tetha - py * sin_tetha;
        vertices[i+1] = px * sin_tetha + py * cos_tetha;
    }

    arrow.index_count = 9;

    glGenBuffers(1, &arrow.VBO), glGenBuffers(1, &arrow.EBO), glGenVertexArrays(1, &arrow.VAO);
    glBindVertexArray(arrow.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, arrow.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrow.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return arrow;
}

static draw_object get_rect(const vec2 size) {
    draw_object rect;
    float vertices[] = {
            -size[0], -size[1],
            size[0], -size[1],
            -size[0], size[1],
            size[0], size[1]
    };

    unsigned int indices[] = {
            0,1,2,
            1,2,3
    };

    rect.index_count = 6;

    glGenBuffers(1, &rect.VBO), glGenBuffers(1, &rect.EBO), glGenVertexArrays(1, &rect.VAO);
    glBindVertexArray(rect.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, rect.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return rect;
}

static draw_object get_circle(const float radius, const int segment_count) {
    draw_object circle;

    int vertex_count = segment_count + 1;
    int vertices_float_count = vertex_count * 2;
    float* vertices = malloc(vertices_float_count * sizeof(float));

    circle.index_count = segment_count * 3;
    unsigned int* indices = malloc(circle.index_count * sizeof(unsigned int));

    float sector_step = (2 * GLM_PIf) / (float) segment_count;

    vertices[0] = 0.0f; vertices[1] = 0.0f;

    for(int i = 0; i < segment_count; ++i) {
        float angle = (float) i * sector_step;
        int k = (i + 1) * 2;

        vertices[k++] = radius * cosf(angle);
        vertices[k++] = radius * sinf(angle);
    }

    for(int i = 0; i < segment_count; ++i) {
        int k = i * 3;
        indices[k] = 0;
        indices[k + 1] = i + 1;
        indices[k + 2] = (i + 1) % segment_count + 1;
    }

    glGenBuffers(1, &circle.VBO), glGenBuffers(1, &circle.EBO), glGenVertexArrays(1, &circle.VAO);
    glBindVertexArray(circle.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, circle.VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices_float_count * sizeof(float)), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (circle.index_count * sizeof(unsigned int)), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
    free(indices);

    return circle;
}

static draw_object get_line2d(const vec2 start, const vec2 end) {

    draw_object line;

    float vertices[] = {
            start[0], start[1],
            end[0], end[1]
    };

    unsigned int indices[] = {
            0, 1,
    };

    line.index_count = 2;

    glGenBuffers(1, &line.VBO), glGenBuffers(1, &line.EBO), glGenVertexArrays(1, &line.VAO);
    glBindVertexArray(line.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, line.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return line;
}

graphics_objects2d get_graphics_objects2d() {
    graphics_objects2d g_objects;

    g_objects.screen_rect = get_rect((vec2){1,1});
    g_objects.circle = get_circle(1, 16);

    g_objects.arrows[DIR2D_X] = get_arrow2d((vec2){1, 0});
    g_objects.arrows[DIR2D_Y] = get_arrow2d((vec2){0, 1});

    g_objects.lines[DIR2D_X] = get_line2d((vec2){0,0}, (vec2){1,0});
    g_objects.lines[DIR2D_Y] = get_line2d((vec2){0,0}, (vec2){0,1});

    return g_objects;
}
