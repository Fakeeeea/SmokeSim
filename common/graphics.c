//
// Created by aaa on 07/03/2026.
//

#include <cglm/cglm.h>

#include "graphics.h"

graphics_settings get_default_g_settings() {
    graphics_settings g_settings = {
            .light_direction = {1.0f, 1.0f, 1.0f, 0.0f},
            .ambient_color = {0.678f, 0.847f, 0.902f, 1.0f},
            .objects_color = {1.0f, 0.0f, 0.0f, 0.0f},
            .light_color = {1.0f * 1.5f, 0.9f * 1.5f, 0.8f * 1.5f},
            .smoke_density_factor = 50.0f,
            .shadow_density_factor = 50.0f,
            .step_size = 0.01f,
            .shadow_step_factor = 3.0f,
            .shadow_steps = 6.0f,
            .z_far = 100.0f,
            .z_near = 0.1f,
            .fov = glm_rad(45),
    };
    return g_settings;
}

graphics_info init_graphics_info() {
    graphics_info g_info;

    g_info.screen_size[0] = INIT_SCREEN_SIZE_X, g_info.screen_size[1] = INIT_SCREEN_SIZE_Y;
    g_info.draw_area[0] = 0; g_info.draw_area[1] = 0; g_info.draw_area[2] = INIT_SCREEN_SIZE_X; g_info.draw_area[3] = INIT_SCREEN_SIZE_Y;

    gen_framebuffer_textures(&g_info.scene_fbo, &g_info.scene_color_tex, &g_info.depth_tex, (ivec2){INIT_SCREEN_SIZE_X, INIT_SCREEN_SIZE_Y});

    g_info.g_info2d = init_graphics_info2d();
    g_info.g_info3d = init_graphics_info3d();

    g_info.g_settings = get_default_g_settings();

    upload_graphics_variables(&g_info);

    return g_info;
}

void upload_graphics_variables(graphics_info* g_info) {
    glGenBuffers(1, &g_info->graphics_variables_ubo);

    mat4 look_at;
    get_lookat(g_info->g_info3d.cam, look_at);

    mat4 projection;
    glm_perspective(g_info->g_settings.fov, (float) g_info->draw_area[2] / (float) g_info->draw_area[3], g_info->g_settings.z_near, g_info->g_settings.z_far, projection);

    mat4 ortho;
    glm_ortho(0, (float) g_info->screen_size[0], 0, (float) g_info->screen_size[1], 0, 0, ortho);

    vec4 cam_pos_vec4 = {g_info->g_info3d.cam.camera_pos[0], g_info->g_info3d.cam.camera_pos[1], g_info->g_info3d.cam.camera_pos[2], 0.0f};

    size_t total_size = 3 * sizeof(mat4) + 2 * sizeof(vec4) + sizeof(graphics_settings);

    glBindBuffer(GL_UNIFORM_BUFFER, g_info->graphics_variables_ubo);
    glBufferData(GL_UNIFORM_BUFFER, total_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), look_at);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), projection);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*2, sizeof(mat4), ortho);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3, sizeof(vec4), (vec4){g_info->draw_area[0],g_info->draw_area[1],g_info->draw_area[2],g_info->draw_area[3]});
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3 + sizeof(vec4), sizeof(vec4), cam_pos_vec4);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3 + sizeof(vec4)*2, sizeof(graphics_settings), &g_info->g_settings);

    glBindBufferBase(GL_UNIFORM_BUFFER, GRAPHICS_VARIABLES_UBO, g_info->graphics_variables_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void update_graphics_variables(const graphics_info* g_info) {
    mat4 look_at;
    get_lookat(g_info->g_info3d.cam, look_at);

    mat4 projection;
    glm_perspective(g_info->g_settings.fov, (float) g_info->draw_area[2]/ (float) g_info->draw_area[3], g_info->g_settings.z_near, g_info->g_settings.z_far, projection);

    mat4 ortho;
    glm_ortho(0, (float) g_info->screen_size[0], 0, (float) g_info->screen_size[1], -1.0f, 1.0f, ortho);

    vec4 cam_pos_vec4 = {g_info->g_info3d.cam.camera_pos[0], g_info->g_info3d.cam.camera_pos[1], g_info->g_info3d.cam.camera_pos[2], 0.0f};

    glBindBuffer(GL_UNIFORM_BUFFER, g_info->graphics_variables_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), look_at);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), projection);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*2, sizeof(mat4), ortho);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3, sizeof(vec4), (vec4){g_info->draw_area[0],g_info->draw_area[1],g_info->draw_area[2],g_info->draw_area[3]});
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3 + sizeof(vec4), sizeof(vec4), cam_pos_vec4);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*3 + sizeof(vec4)*2, sizeof(graphics_settings), &g_info->g_settings);
}

void graphics_clear(const graphics_info* g_info) {
    glClearColor(g_info->g_settings.ambient_color[0], g_info->g_settings.ambient_color[1], g_info->g_settings.ambient_color[2], g_info->g_settings.ambient_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void enable_depth_blend() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDepthMask(GL_TRUE);
}
void disable_depth() {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
}

void draw_step(grid* grid, graphics_info* g_info, physics_info* p_info) {
    static unsigned int ubo = 0;

    update_graphics_variables(g_info);

    glBindFramebuffer(GL_FRAMEBUFFER, g_info->scene_fbo);

    viewport_draw_area(g_info);

    enable_depth_blend();
    graphics_clear(g_info);

    if(grid->is_2d) {
        graphics_step_settings2d* g_s = &g_info->g_info2d.g_s_settings;

        if(g_s->draw_smoke) draw_smoke2d(&g_info->g_info2d);
        if(g_s->draw_obstacles) draw_obstacles2d(&g_info->g_info2d, p_info->o_info.obstacles_count);

        blit_framebuffer(g_info);
    } else {
        graphics_step_settings3d* g_s = &g_info->g_info3d.g_s_settings;

        if(g_s->draw_direction) draw_direction3d(&g_info->g_info3d);
        if(g_s->draw_obstacles) draw_obstacles3d(&g_info->g_info3d, p_info->o_info.obstacles_count);
        if(g_s->draw_grid_lines) draw_grid3d(&g_info->g_info3d, grid->grid3d_data.size);
        if(g_s->draw_velocities) draw_velocity_arrows3d(&g_info->g_info3d, grid->grid3d_data.size);
        if(g_s->draw_vorticity) draw_vorticity3d(&g_info->g_info3d, grid->grid3d_data.size);
        blit_framebuffer(g_info);
        disable_depth();

        glBindTextureUnit(DEPTH_TEX, g_info->depth_tex.ID);
        if(g_s->draw_smoke) draw_smoke3d(&g_info->g_info3d);
    }
}

void blit_framebuffer(const graphics_info* g_info) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_info->scene_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, (int)g_info->screen_size[0], (int)g_info->screen_size[1],
                      0, 0, (int)g_info->screen_size[0], (int)g_info->screen_size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_area_reset(graphics_info* g_info) {
    g_info->draw_area[0] = 0;
    g_info->draw_area[1] = 0;
    g_info->draw_area[2] = g_info->screen_size[0];
    g_info->draw_area[3] = g_info->screen_size[1];
}

void viewport_draw_area(graphics_info* g_info) {
    glViewport((int) g_info->draw_area[0], (int) g_info->draw_area[1], (int) g_info->draw_area[2], (int) g_info->draw_area[3]);
}

void viewport_reset(graphics_info* g_info) {
    glViewport(0, 0, (int) g_info->screen_size[0], (int) g_info->screen_size[1]);
}