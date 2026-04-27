//
// Created by aaa on 22/04/2026.
//

#include "graphics_objects3d.h"

#include <glad/glad.h>

static draw_object get_box(const vec3 bottom_left_back,const vec3 top_right_front) {
    draw_object box;

    float vertices[] = {
            bottom_left_back[0], bottom_left_back[1], bottom_left_back[2],
            top_right_front[0], bottom_left_back[1], bottom_left_back[2],
            top_right_front[0], bottom_left_back[1], top_right_front[2],
            bottom_left_back[0], bottom_left_back[1], top_right_front[2],

            bottom_left_back[0], top_right_front[1], bottom_left_back[2],
            top_right_front[0], top_right_front[1], bottom_left_back[2],
            top_right_front[0], top_right_front[1], top_right_front[2],
            bottom_left_back[0], top_right_front[1], top_right_front[2],
    };

    unsigned int indices[] = {
            //bottom face
            0,1,2,
            0,2,3,

            //front face
            0,1,5,
            0,5,4,

            //left face
            0,3,4,
            3,4,7,

            //right face
            1,2,5,
            2,5,6,

            //back face
            2,3,6,
            3,6,7,

            //top face
            5,4,6,
            6,4,7
    };

    box.index_count = 36;

    glGenBuffers(1, &box.VBO), glGenBuffers(1, &box.EBO), glGenVertexArrays(1, &box.VAO);
    glBindVertexArray(box.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, box.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return box;
}

static draw_object get_sphere(const float radius, const int sector_count, const int stack_count) {
    draw_object sphere;

    float x,y,z, nx, ny, nz, xy;
    float length_inverse = 1.0f/radius;

    int vertices_float_count = (stack_count + 1) * (sector_count + 1) * 3 * 2;
    float* vertices = malloc(vertices_float_count * sizeof(float));

    sphere.index_count = stack_count * sector_count * 6;
    unsigned int* indices = malloc(sphere.index_count * sizeof(unsigned int));

    float sector_step = (2 * GLM_PIf) / (float) sector_count;
    float stack_step = M_PI / stack_count;

    int k = 0;

    for(int i = 0; i <= stack_count; ++i) {
        float stack_angle = M_PI_2 - (float) i * stack_step;
        xy = radius * cosf(stack_angle);
        z = radius * sinf(stack_angle);

        for(int j = 0; j <= sector_count; ++j) {
            float sector_angle = (float) j * sector_step;

            x = xy * cosf(sector_angle);
            y = xy * sinf(sector_angle);

            vertices[k++] = x; vertices[k++] = y; vertices[k++] = z;

            nx = x * length_inverse;
            ny = y * length_inverse;
            nz = z * length_inverse;

            vertices[k++] = nx; vertices[k++] = ny; vertices[k++] = nz;
        }
    }

    k = 0;

    for(int i = 0; i < stack_count; ++i) {
        int begin = i * (sector_count + 1);
        int end = begin + sector_count + 1;

        for(int j = 0; j < sector_count; ++j, ++begin, ++end) {
            if(i != 0) {
                indices[k++] = begin;
                indices[k++] = end;
                indices[k++] = begin + 1;
            }

            if( i != (stack_count - 1)) {
                indices[k++] = begin + 1;
                indices[k++] = end;
                indices[k++] = end + 1;
            }
        }
    }

    glGenBuffers(1, &sphere.VBO), glGenBuffers(1, &sphere.EBO), glGenVertexArrays(1, &sphere.VAO);
    glBindVertexArray(sphere.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices_float_count * sizeof(float)), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (sphere.index_count * sizeof(unsigned int)), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
    free(indices);

    return sphere;
}

static draw_object get_arrow3d(const vec3 pointing_to) {
    draw_object arrow;

    float vertices[] = {
            //TAIL
            -0.05f, 0.0f, 0.0f,
            0.05f, 0.0f, 0.0f,
            -0.05f, 0.6f, 0.0f,
            0.05f, 0.6f, 0.0f,

            //HEAD
            -0.2f, 0.6f, 0.0f,
            0.2f, 0.6f, 0.0f,
            0.0f, 0.8f, 0.0f,
    };

    unsigned int indices[] = {
            //TAIL
            0,1,2,
            1,2,3,

            //HEAD
            4,5,6
    };

    vec3 base_direction = {0.0f, 1.0f, 0.0f};
    vec3 normalized_pointing; glm_vec3_copy((float*)pointing_to, normalized_pointing); glm_normalize(normalized_pointing);

    mat4 rot; glm_mat4_identity(rot);

    float d = glm_vec3_dot(base_direction, normalized_pointing);

    if (d < 0.9999f) {
        vec3 axis;
        glm_vec3_cross(base_direction, normalized_pointing, axis);
        glm_normalize(axis);

        float angle = acosf(d);
        glm_rotate(rot, angle, axis);
    }


    const int POINTS = 7;
    for(int i = 0; i < POINTS * 3; i+=3) {
        vec3 point = {vertices[i], vertices[i+1], vertices[i+2]};
        vec3 rotated;
        glm_mat4_mulv3(rot, point, 1.0f, rotated);
        vertices[i] = rotated[0], vertices[i+1] = rotated[1], vertices[i+2] = rotated[2];
    }

    arrow.index_count = 9;

    glGenBuffers(1, &arrow.VBO), glGenBuffers(1, &arrow.EBO), glGenVertexArrays(1, &arrow.VAO);
    glBindVertexArray(arrow.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, arrow.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrow.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return arrow;
}

static draw_object get_line(const vec3 start, const vec3 end) {
    draw_object line;
    float vertices[] = {
            start[0], start[1], start[2],
            end[0], end[1], end[2]
    };

    unsigned int indices[] = {
            0,1
    };

    line.index_count = 2;

    glGenBuffers(1, &line.VBO), glGenBuffers(1, &line.EBO), glGenVertexArrays(1, &line.VAO);
    glBindVertexArray(line.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, line.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return line;
}

graphics_objects3d get_graphics_objects3d() {
    graphics_objects3d g_objects;

    g_objects.arrows[DIR3D_X] = get_arrow3d((vec3) {1, 0, 0});
    g_objects.arrows[DIR3D_Y] = get_arrow3d((vec3) {0, 1, 0});
    g_objects.arrows[DIR3D_Z] = get_arrow3d((vec3) {0, 0, 1});

    g_objects.lines[DIR3D_X] = get_line((vec3){0,0,0}, (vec3){1,0,0});
    g_objects.lines[DIR3D_Y] = get_line((vec3){0,0,0}, (vec3){0,1,0});
    g_objects.lines[DIR3D_Z] = get_line((vec3){0, 0, 0}, (vec3){0, 0, 1});

    g_objects.grid_box = get_box((vec3){0,0,0}, (vec3){1,1,1});
    g_objects.sphere = get_sphere(1, 16, 16);

    return g_objects;
}
