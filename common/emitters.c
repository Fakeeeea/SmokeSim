//
// Created by aaa on 20/04/2026.
//

#include "emitters.h"
#include "opengl_bindings.h"

#include <glad/glad.h>

emitters_info init_emitters_info() {
    return (emitters_info) {
        .emitters_count = 0,
        .emitters_array = NULL,
        .emitters_ssbo = 0,
    };
}

void handle_emitter_creation(unsigned int SSBO, emitter** emitters, int* emitter_count) {
    (*emitter_count)++;
    size_t emitters_size = *emitter_count * sizeof(emitter);
    *emitters = realloc(*emitters, emitters_size);

    (*emitters)[(*emitter_count)-1] = get_emitter((ivec3){0,0,0}, 1, (ivec3){0,0,0}, 0, 0, 1);

    size_t new_size = 16 + emitters_size;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), emitter_count);

    update_emitters(SSBO, *emitters, *emitter_count);
}

void handle_emitter_deletion(unsigned int SSBO, emitter** emitters, int* emitter_count, const int deleted_id) {
    for(int i = deleted_id; i < (*emitter_count) -1; ++i) {
        (*emitters)[i] = (*emitters)[i+1];
    }

    (*emitter_count)--;

    size_t emitters_size = sizeof(emitter) * (*emitter_count);
    *emitters = realloc(*emitters, emitters_size);

    size_t new_size = emitters_size + 16;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, new_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), emitter_count);

    update_emitters(SSBO, *emitters, *emitter_count);
}

emitter get_emitter(const ivec3 pos, const int radius, const ivec3 density, const int temperature, const int delta_time, const int has_falloff) {
    emitter e = { .pos_radius = {pos[0], pos[1], pos[2], radius},
                  .density_temp = {density[0], density[1], density[2], temperature},
                  .emitter_settings = {0, delta_time, 0, has_falloff}
    };
    return e;
}

unsigned int upload_emitters(const emitter* emitters, int emitter_count) {
    unsigned int SSBO;
    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

    size_t emitters_size = emitter_count * sizeof(emitter);
    size_t size = 16 + emitters_size;

    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &emitter_count);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, emitters_size, emitters);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, EMITTERS_SSBO, SSBO);

    return SSBO;
}

void update_emitters(unsigned int SSBO, const emitter* emitters, int emitter_count) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    size_t emitters_size = emitter_count * sizeof(emitter);

    if(emitter_count == 0) return;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, emitters_size, emitters);
}