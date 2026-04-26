//
// Created by aaa on 20/04/2026.
//

#ifndef SMOKESIM_EMITTERS_H
#define SMOKESIM_EMITTERS_H

#include <cglm/cglm.h>

typedef struct emitter {
    ivec4 pos_radius;
    ivec4 density_temp;
    ivec4 emitter_settings; //x - delay, y - delta
}emitter;

typedef struct emitters_info {
    int emitters_count;
    emitter* emitters_array;
    unsigned int emitters_ssbo;
}emitters_info;

emitters_info init_emitters_info();

void handle_emitter_creation(unsigned int SSBO, emitter** emitters, int* emitter_count);
void handle_emitter_deletion(unsigned int SSBO, emitter** emitters, int* emitter_count, int deleted_id);

emitter get_emitter(const ivec3 pos, int radius, const ivec3 density, int temperature, int delta_time, int has_falloff);

unsigned int upload_emitters(const emitter* emitters, int emitter_count);
void update_emitters(unsigned int SSBO, const emitter* emitters, int emitter_count);

#endif //SMOKESIM_EMITTERS_H
