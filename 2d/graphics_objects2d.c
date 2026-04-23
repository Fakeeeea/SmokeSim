//
// Created by aaa on 22/04/2026.
//

#include "graphics_objects2d.h"

#include <glad/glad.h>

static draw_object get_rect(const vec2 size) {
    draw_object rect;
    float vertices[] = {
            0, 0,
            size[0], 0,
            0, size[1],
            size[0], size[1]
    };

    unsigned int indices[] = {
            0,1,2,
            0,2,3
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

graphics_objects2d get_graphics_objects2d() {
    graphics_objects2d g_objects;

    g_objects.screen_rect = get_rect((vec2){1,1});

    return g_objects;
}
