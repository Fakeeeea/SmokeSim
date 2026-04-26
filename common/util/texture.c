//
// Created by aaa on 24/03/2026.
//

#include "texture.h"
#include <glad/glad.h>

texture2 create_texture2_buf(const ivec2 size, int internal_format, int format, unsigned int data_type, int filtering, const void* buffer, int clamping_type, const void* border_value) {
    texture2 t;

    glGenTextures(1, &t.ID);
    glm_ivec2_copy(size, t.size);
    t.data_type = data_type, t.format = format, t.internal_format = internal_format;

    glBindTexture(GL_TEXTURE_2D, t.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamping_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamping_type);

    if(clamping_type == GL_CLAMP_TO_BORDER && border_value != NULL) {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_value);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size[0], size[1], 0, format, data_type, buffer);
    return t;
}

texture2 create_texture2(const ivec2 size, int internal_format, int format, unsigned int data_type, int filtering, const void* initialization_value, int clamping_type, const void* border_value) {
    texture2 t;

    glGenTextures(1, &t.ID);
    glm_ivec2_copy(size, t.size);
    t.data_type = data_type, t.format = format, t.internal_format = internal_format;

    glBindTexture(GL_TEXTURE_2D, t.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamping_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamping_type);

    if(clamping_type == GL_CLAMP_TO_BORDER && border_value != NULL) {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_value);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size[0], size[1], 0, format, data_type, NULL);
    glClearTexImage(t.ID, 0, format, data_type, initialization_value);

    return t;
}

texture3 create_texture3(const ivec3 size, int internal_format, int format, unsigned int data_type, int filtering, const void* initialization_value, int clamping_type, const void* border_value) {
    texture3 t;

    glGenTextures(1, &t.ID);
    glm_ivec3_copy(size, t.size);
    t.data_type = data_type, t.format = format, t.internal_format = internal_format;

    glBindTexture(GL_TEXTURE_3D, t.ID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filtering);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, clamping_type);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, clamping_type);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, clamping_type);

    if(clamping_type == GL_CLAMP_TO_BORDER && border_value != NULL) {
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, border_value);
    }

    glTexImage3D(GL_TEXTURE_3D, 0, internal_format, size[0], size[1], size[2], 0, format, data_type, NULL);
    glClearTexImage(t.ID, 0, format, data_type, initialization_value);

    return t;
}

void gen_framebuffer_textures(unsigned int* scene_fbo, texture2* scene_color_tex, texture2* depth_tex, const ivec2 size) {
    glGenFramebuffers(1, scene_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *scene_fbo);
    *scene_color_tex = create_texture2(size, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, NULL, GL_CLAMP_TO_EDGE, NULL);
    *depth_tex = create_texture2(size, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, NULL, GL_CLAMP_TO_EDGE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_color_tex->ID, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex->ID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("There was an error generating the frame buffer!");
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
