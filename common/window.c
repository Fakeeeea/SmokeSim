//
// Created by aaa on 22/03/2026.
//

#include "window.h"
#include "../3d/camera3d.h"
#include "nk_styles.h"

void init_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLTRUE);
#endif
}

GLFWwindow* create_window() {
    GLFWwindow* window = glfwCreateWindow(INIT_SCREEN_SIZE_X, INIT_SCREEN_SIZE_Y, "SmokeSim", NULL, NULL);

    if(window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
    }

    return window;
}

int load_glad() {
    int e;
    e = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if(!e) {
        printf("Failed to initialize GLAD");
        glfwTerminate();
    }
    return e;
}

void setup_callbacks() {
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(ctx.window, framebuffer_size_callback);
    glfwSetScrollCallback(ctx.window, scroll_wheel_callback);
    glfwSetCursorPosCallback(ctx.window, mouse_callback);
    glfwSetKeyCallback(ctx.window, key_press_callback);

    glfwSetMouseButtonCallback(ctx.window, mouse_button_callback);
    glfwSetCharCallback(ctx.window, nk_glfw3_char_callback);
}

void init_window_context(GLFWwindow* window, struct nk_context* nk_ctx, grid* grid, graphics_info* g_info, physics_info* p_info, physics_info* mm_p_info, main_menu_info* mm_info) {
    ctx = (window_context){
        .window = window,

        .g_info = g_info,
        .p_info = p_info,
        .mm_info = mm_info,
        .grid = grid,
        .mm_p_info = mm_p_info,

        .m_info = {0,0,0,0,1},
        .mov_info = {1,1,2},

        .nk_ctx = nk_ctx,

        .init_grid = 0,
        .created_grid = 0,

        .settings_open = 0,

        .is_obstacles_window_open = 0,
        .is_emitters_window_open = 0,
        };
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
    ctx.g_info->screen_size[0] = width;
    ctx.g_info->screen_size[1] = height;

    glDeleteTextures(1, &ctx.g_info->scene_color_tex.ID);
    glDeleteTextures(1, &ctx.g_info->depth_tex.ID);
    glGenFramebuffers(1, &ctx.g_info->scene_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx.g_info->scene_fbo);
    ctx.g_info->scene_color_tex = create_texture2((ivec2){width, height}, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, NULL, GL_CLAMP_TO_EDGE, NULL);
    ctx.g_info->depth_tex = create_texture2((ivec2){width, height}, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, NULL, GL_CLAMP_TO_EDGE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx.g_info->scene_color_tex.ID, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ctx.g_info->depth_tex.ID, 0);
}

void process_input(){
    if(glfwGetKey(ctx.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(ctx.window, 1);
}

void scroll_wheel_callback(GLFWwindow *window, double xoffset, double yoffset) {

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        const float CAMERA_SPEED_FREECAM = 0.2f;
        const float CAMERA_SPEED_ORBIT = 0.05f;
        if(ctx.mov_info.orbit_mode) {
            ctx.mov_info.orbit_dist -= CAMERA_SPEED_ORBIT * yoffset;
            mouse_movement_orbit(&ctx.g_info->g_info3d.cam, (ivec3){ctx.grid->grid3d_data.size[0],
                                                           ctx.grid->grid3d_data.size[1],
                                                           ctx.grid->grid3d_data.size[2]}, ctx.mov_info.orbit_dist, 0, 0);
        }
        else
            move_camera_z(&ctx.g_info->g_info3d.cam, CAMERA_SPEED_FREECAM * yoffset);
    } else {
        nk_gflw3_scroll_callback(window, xoffset, yoffset);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {

    ctx.m_info.pos_x = xpos; ctx.m_info.pos_y = ypos;

    if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED){
        return;
    }

    if(ctx.m_info.first_mouse) {
        ctx.m_info.last_x = xpos;
        ctx.m_info.last_y = ypos;
        ctx.m_info.first_mouse = 0;
    }

    float x_offset = xpos - ctx.m_info.last_x;
    float y_offset = ctx.m_info.last_y - ypos;

    ctx.m_info.last_x = xpos;
    ctx.m_info.last_y = ypos;

    if(ctx.mov_info.orbit_mode) {
        mouse_movement_orbit(&ctx.g_info->g_info3d.cam, (ivec3){ctx.grid->grid3d_data.size[0],
                                                       ctx.grid->grid3d_data.size[1],
                                                       ctx.grid->grid3d_data.size[2]}, ctx.mov_info.orbit_dist, x_offset, y_offset);
    } else {
        mouse_movement(&ctx.g_info->g_info3d.cam, x_offset, y_offset);
    }

}

void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_S:
                ctx.settings_open = !ctx.settings_open;
                if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    ctx.m_info.first_mouse = 1;
                }
                break;
            case GLFW_KEY_P:
                ctx.p_info->paused = !ctx.p_info->paused;
                break;
            case GLFW_KEY_O:
                ctx.mov_info.orbit_mode = !ctx.mov_info.orbit_mode;
                ctx.m_info.first_mouse = 1;
                break;
            case GLFW_KEY_A:
                ctx.mov_info.auto_orbit = !ctx.mov_info.auto_orbit;
                break;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    if(action == GLFW_PRESS) {
        switch(handle_click(ctx.mm_info, (vec2){ctx.m_info.pos_x,ctx.g_info->screen_size[1]-ctx.m_info.pos_y})) {
            case NEW_SIM_ID:
                ctx.mm_info->state = MM_OPTIONS;
                init_new_grid_screen();
                break;
            case LOAD_ID:
                printf("Pressed load sim!");
                break;
        }
    }
    nk_glfw3_mouse_button_callback(window, button, action, mods);
}

//TODO HANDLE HOVER EVERY STEP

int draw_gui() {
    } else if(ctx.mm_info->state == MM_OPTIONS) {
        draw_creation_settings();
        return 0;
    }

    draw_camera_info();

    if(ctx.settings_open == 0) return 0;

    int re_upload = 0;

    if(nk_begin(ctx.nk_ctx, "Settings", nk_rect(50,50,300,600),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

        re_upload = draw_physics_settings();
        draw_physics_step_settings();
        draw_graphics_settings();
        draw_graphics_step_settings();

        nk_checkbox_label(ctx.nk_ctx, "Emitters GUI", &ctx.is_emitters_window_open);
        nk_checkbox_label(ctx.nk_ctx, "Obstacles GUI", &ctx.is_obstacles_window_open);
        nk_checkbox_label(ctx.nk_ctx, "Pause simulation", &ctx.p_info->paused);
    }
    nk_end(ctx.nk_ctx);

    if(ctx.is_emitters_window_open) {
        if(nk_begin(ctx.nk_ctx, "Emitters Settings", nk_rect(350, 50, 200, 300),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
            draw_emitters_gui();
        }
        nk_end(ctx.nk_ctx);
    }

    if(ctx.is_obstacles_window_open) {
        if(nk_begin(ctx.nk_ctx, "Obstacles Settings", nk_rect(350 , 350, 200, 300),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
            draw_obstacles_gui();
        }
        nk_end(ctx.nk_ctx);
    }

    return re_upload;
}

void draw_graphics_settings() {
    graphics_settings* g_settings = &ctx.g_info->g_settings;
    if(nk_tree_push(ctx.nk_ctx, NK_TREE_TAB, "Graphics Settings", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        nk_property_float(ctx.nk_ctx, "#Smoke Density Factor:", 0.0f, &g_settings->smoke_density_factor, 200.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Shadow Density Factor:", 0.0f, &g_settings->shadow_density_factor, 200.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Ray March Step Size:", 0.001f, &g_settings->step_size, 0.2f, 0.001f, 0.001f);
        nk_property_float(ctx.nk_ctx, "#Ray March Shadow Step Factor:", 1.0f, &g_settings->shadow_step_factor, 20.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Ray March Shadow Steps:", 1.0f, &g_settings->shadow_steps, 20.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Z Far:", 0.001f, &g_settings->z_far, 1000.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Z Near:", 0.001f, &g_settings->z_near, 1000.0f, 1.0f, 1.0f);
        nk_property_float(ctx.nk_ctx, "#Fov:", glm_rad(1), &g_settings->fov, glm_rad(90), glm_rad(1), glm_rad(1));

        nk_label(ctx.nk_ctx, "Light Direction", NK_TEXT_LEFT);
        nk_property_float(ctx.nk_ctx, "#X:", 0.0f, &g_settings->light_direction[0], 1.0f, 0.01f, 0.01f);
        nk_property_float(ctx.nk_ctx, "#Y:", 0.0f, &g_settings->light_direction[1], 1.0f, 0.01f, 0.01f);
        nk_property_float(ctx.nk_ctx, "#Z:", 0.0f, &g_settings->light_direction[2], 1.0f, 0.01f, 0.01f);

        draw_color_picker("Ambient Color: ", g_settings->ambient_color, 1.0f);
        draw_color_picker("Light Color: ", g_settings->light_color, 1.5f);
        draw_color_picker("Object Color: ", g_settings->objects_color, 1.0f);

        nk_tree_pop(ctx.nk_ctx);
    }
}

void draw_graphics_step_settings() {
    graphics_step_settings3d* g_s_settings = &ctx.g_info->g_info3d.g_s_settings;
    if(nk_tree_push(ctx.nk_ctx, NK_TREE_TAB, "Graphics Steps", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        nk_checkbox_label(ctx.nk_ctx, "Draw Grid Lines", &g_s_settings->draw_grid_lines);
        nk_checkbox_label(ctx.nk_ctx, "Draw Velocities", &g_s_settings->draw_velocities);
        nk_checkbox_label(ctx.nk_ctx, "Draw Vorticity", &g_s_settings->draw_vorticity);
        nk_checkbox_label(ctx.nk_ctx, "Draw Smoke", &g_s_settings->draw_smoke);
        nk_checkbox_label(ctx.nk_ctx, "Draw Obstacles", &g_s_settings->draw_obstacles);

        nk_tree_pop(ctx.nk_ctx);
    }
}

int draw_physics_settings() {
    int change = 0;
    physics_settings* p_settings = &ctx.p_info->p_settings;
    if(nk_tree_push(ctx.nk_ctx, NK_TREE_TAB, "Physics Constants", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        change += nk_property_float(ctx.nk_ctx, "#Time Step:", 0.0001f, &p_settings->time_step, 0.1f, 0.001f, 0.001f);
        change += nk_property_float(ctx.nk_ctx, "#Density:", 0.1f, &p_settings->density, 10.0f, 0.1f, 0.05f);
        change += nk_property_float(ctx.nk_ctx, "#Ambient Temp:", 0.0f, &p_settings->t_ambient, 500.0f, 1.0f, 1.0f);
        change += nk_property_float(ctx.nk_ctx, "#Vorticity Confinement:", 0.0f, &p_settings->vorticity_confinement, 10.0f, 0.1f, 0.05f);
        change += nk_property_float(ctx.nk_ctx, "#Temp Coefficient:", 0.0f, &p_settings->t_temp_coeff, 10.0f, 0.01f, 0.05f);
        change += nk_property_float(ctx.nk_ctx, "#Weight Coefficient:", 0.0f, &p_settings->t_weight_coeff, 10.0f, 0.01f, 0.05f);

        change += nk_property_float(ctx.nk_ctx, "#Wind X", -5.0f, &p_settings->wind[0], 5.0f, 0.2f, 0.2f);
        change += nk_property_float(ctx.nk_ctx, "#Wind Y", -5.0f, &p_settings->wind[1], 5.0f, 0.2f, 0.2f);
        change += nk_property_float(ctx.nk_ctx, "#Wind Z", -5.0f, &p_settings->wind[2], 5.0f, 0.2f, 0.2f);

        nk_tree_pop(ctx.nk_ctx);
    }
    return change > 0;
}

void draw_physics_step_settings() {
    physics_step_settings* p_s_settings = &ctx.p_info->p_s_settings;
    if(nk_tree_push(ctx.nk_ctx, NK_TREE_TAB, "Simulation Steps", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        nk_checkbox_label(ctx.nk_ctx, "Handle Emitters", &p_s_settings->handle_emitters);
        nk_checkbox_label(ctx.nk_ctx, "Apply Vorticity", &p_s_settings->apply_vorticity);
        nk_checkbox_label(ctx.nk_ctx, "Apply Buoyancy", &p_s_settings->apply_buoyancy);
        nk_checkbox_label(ctx.nk_ctx, "Resolve Pressure", &p_s_settings->resolve_pressure);
        nk_checkbox_label(ctx.nk_ctx, "Update Velocities", &p_s_settings->update_velocities);
        nk_checkbox_label(ctx.nk_ctx, "Advect Velocities", &p_s_settings->advect_velocities);
        nk_checkbox_label(ctx.nk_ctx, "Advect Smoke", &p_s_settings->advect_smoke);

        nk_tree_pop(ctx.nk_ctx);
    }
}

void draw_emitters_gui() {
    int change = 0;
    struct nk_style_button creation_style = get_creation_style(ctx.nk_ctx);

    emitters_info* e_info_ptr = &ctx.p_info->e_info;

    for(int i = 0; i < e_info_ptr->emitters_count; ++i) {
        int current_change = draw_emitter_info(i);
        if(current_change == -1) {
            handle_emitter_deletion(e_info_ptr->emitters_ssbo, &e_info_ptr->emitters_array, &e_info_ptr->emitters_count, i);
        } else {
            change += current_change;
        }
    }

    if(change > 0) {
        update_emitters(e_info_ptr->emitters_ssbo, e_info_ptr->emitters_array, e_info_ptr->emitters_count);
    }

    nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
    if(nk_button_text_styled(ctx.nk_ctx, &creation_style, "Create new emitter", 18)) {
        handle_emitter_creation(e_info_ptr->emitters_ssbo, &e_info_ptr->emitters_array, &e_info_ptr->emitters_count);
    }
}

int draw_emitter_info(int e_num) {
    int change = 0;
    char buf[64];
    sprintf(buf, "Emitter %i", e_num);

    char prop_x[32], prop_y[32], prop_z[32];
    sprintf(prop_x, "#X (E%d)", e_num); sprintf(prop_y, "#Y (E%d)", e_num); sprintf(prop_z, "#Z (E%d)", e_num);

    emitter* e = &(ctx.p_info->e_info.emitters_array[e_num]);

    struct nk_style_button delete_style = get_delete_style(ctx.nk_ctx);
    if(nk_tree_push_id(ctx.nk_ctx, NK_TREE_TAB, buf, NK_MINIMIZED, e_num)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        nk_label(ctx.nk_ctx, "Position:", NK_TEXT_LEFT);
        change += nk_property_int(ctx.nk_ctx, prop_x, -9999, &e->pos_radius[0], 9999, 1, 1);
        change += nk_property_int(ctx.nk_ctx, prop_y, -9999, &e->pos_radius[1], 9999, 1, 1);
        change += nk_property_int(ctx.nk_ctx, prop_z, -9999, &e->pos_radius[2], 9999, 1, 1);
        change += nk_property_int(ctx.nk_ctx, "#Radius", 1, &e->pos_radius[3], 9999, 1, 1);
        nk_label(ctx.nk_ctx, "Densities:", NK_TEXT_LEFT);
        change += nk_property_int(ctx.nk_ctx, "#R", 0, &e->density_temp[0], 1000000, 1, 1);
        change += nk_property_int(ctx.nk_ctx, "#G", 0, &e->density_temp[1], 1000000, 1, 1);
        change += nk_property_int(ctx.nk_ctx, "#B", 0, &e->density_temp[2], 1000000, 1, 1);
        change += nk_property_int(ctx.nk_ctx, "#Temperature delta", -9999, &e->density_temp[3], 1000000, 1, 1);
        change += nk_property_int(ctx.nk_ctx, "#Delay between emits", 0, &e->emitter_settings[0], 100000, 1, 1);
        change += nk_checkbox_label(ctx.nk_ctx, "#Falloff?", &e->emitter_settings[3]);
        if(nk_button_text_styled(ctx.nk_ctx, &delete_style, "Delete Emitter", 14)) {
            change = -1;
        }
        nk_tree_pop(ctx.nk_ctx);
    }
    return change;
}

void draw_obstacles_gui() {
    static int sync_continuously = 0;
    int change = 0;

    struct nk_style_button creation_style = get_creation_style(ctx.nk_ctx);

    obstacles_info* o_info_ptr = &ctx.p_info->o_info;

    nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);

    if(nk_button_text(ctx.nk_ctx, "Sync GPU - CPU data", 19) || sync_continuously && o_info_ptr->obstacles_count != 0) {
        extract_obstacles_data(o_info_ptr->obstacles_array, o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
    }

    nk_checkbox_label(ctx.nk_ctx, "Sync data continuously", &sync_continuously);

    for(int i = 0; i < o_info_ptr->obstacles_count; ++i) {
        int current_change = draw_obstacle_info(i);
        if(current_change == -1) {
            handle_obstacle_deletion(&o_info_ptr->obstacles_array, &o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos, i);
        } else {
            change += current_change;
        }
    }

    if(change > 0) {
        update_obstacles(o_info_ptr->obstacles_array, o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
    }

    if(nk_button_text_styled(ctx.nk_ctx, &creation_style, "Create new obstacle", 19)) {
        handle_obstacle_creation(&o_info_ptr->obstacles_array, &o_info_ptr->obstacles_count, o_info_ptr->obstacles_ssbos);
    }
}

int draw_obstacle_info(int o_num) {
    int change = 0;
    char buf[64];
    sprintf(buf, "Obstacle %i", o_num);

    char prop_x[32], prop_y[32], prop_z[32];
    sprintf(prop_x, "#X (O%d)", o_num); sprintf(prop_y, "#Y (O%d)", o_num); sprintf(prop_z, "#Z (O%d)", o_num);

    obstacle* o = &(ctx.p_info->o_info.obstacles_array[o_num]);
    int anchored_val = o->velocities_anchored[3];

    struct nk_style_button delete_style = get_delete_style(ctx.nk_ctx);
    if(nk_tree_push_id(ctx.nk_ctx, NK_TREE_TAB, buf, NK_MINIMIZED, o_num)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 15, 1);
        nk_label(ctx.nk_ctx, "Position:", NK_TEXT_LEFT);
        change += nk_property_float(ctx.nk_ctx, prop_x, -9999, &o->pos_radius[0], 9999, 1, 1);
        change += nk_property_float(ctx.nk_ctx, prop_y, -9999, &o->pos_radius[1], 9999, 1, 1);
        change += nk_property_float(ctx.nk_ctx, prop_z, -9999, &o->pos_radius[2], 9999, 1, 1);
        change += nk_property_float(ctx.nk_ctx, "Radius", 1, &o->pos_radius[3], 9999, 1, 1);
        nk_label(ctx.nk_ctx, "Velocities:", NK_TEXT_LEFT);
        change += nk_property_float(ctx.nk_ctx, prop_x, 0, &o->velocities_anchored[0], 100000, 1, 1);
        change += nk_property_float(ctx.nk_ctx, prop_y, 0, &o->velocities_anchored[1], 100000, 1, 1);
        change += nk_property_float(ctx.nk_ctx, prop_z, 0, &o->velocities_anchored[2], 100000, 1, 1);
        change += nk_checkbox_label(ctx.nk_ctx, "Anchored", &anchored_val);
        if(nk_button_text_styled(ctx.nk_ctx, &delete_style, "Delete Obstacle", 15)) {
            change = -1;
        }
        nk_tree_pop(ctx.nk_ctx);
    }

    o->velocities_anchored[3] = anchored_val;
    return change;
}

void init_new_grid_screen() {
    init_placeholder_grid(ctx.grid);
    *ctx.p_info = init_physics_info(ctx.mm_p_info->p_shaders);

    ctx.g_info->g_info3d.g_s_settings.draw_grid_lines = 1;

    ctx.init_grid = 1;
}

void draw_camera_info() {

    nk_style_push_color(ctx.nk_ctx, &ctx.nk_ctx->style.window.fixed_background.data.color, nk_rgba(0,0,0,0));
    nk_style_push_float(ctx.nk_ctx, &ctx.nk_ctx->style.window.border, 0.0f);

    if(nk_begin(ctx.nk_ctx, "Camera", nk_rect(ctx.g_info->screen_size[0] - 200,0,200,100), NK_WINDOW_NOT_INTERACTIVE | NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx.nk_ctx, 30, 1);
        nk_label(ctx.nk_ctx, ctx.mov_info.orbit_mode ? "Camera ORBIT on (O)" : "Camera ORBIT off (O)", NK_TEXT_RIGHT);
        nk_label(ctx.nk_ctx, ctx.mov_info.auto_orbit ? "AUTO ORBIT on (A)" : "AUTO ORBIT off (A)", NK_TEXT_RIGHT);
    }
    nk_end(ctx.nk_ctx);

    nk_style_pop_float(ctx.nk_ctx);
    nk_style_pop_color(ctx.nk_ctx);
}
