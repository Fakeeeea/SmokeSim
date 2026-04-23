//
// Created by aaa on 24/03/2026.
//

#ifndef SMOKESIM_TEXTURE_H
#define SMOKESIM_TEXTURE_H

#include <cglm/cglm.h>

typedef struct texture2 {
    unsigned int ID;
    unsigned int data_type;
    unsigned int internal_format;
    unsigned int format;
    ivec2 size;
}texture2;

typedef struct texture3 {
    unsigned int ID;
    unsigned int data_type;
    unsigned int internal_format;
    unsigned int format;
    ivec3 size;
}texture3;

texture2 create_texture2_buf(const ivec2 size, int internal_format, int format, unsigned int data_type, int filtering, const void* buffer, int clamping_type, const void* border_value);
texture2 create_texture2(const ivec2 size, int internal_format, int format, unsigned int data_type, int filtering, const void* initialization_value, int clamping_type, const void* border_value);
texture3 create_texture3(const ivec3 size, int internal_format, int format, unsigned int data_type, int filtering, const void* initialization_value, int clamping_type, const void* border_value);

void gen_framebuffer_textures(unsigned int* scene_fbo, texture2* scene_color_tex, texture2* depth_tex, const ivec2 size);

#endif //SMOKESIM_TEXTURE_H
