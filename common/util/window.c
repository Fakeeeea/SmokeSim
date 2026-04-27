//
// Created by aaa on 22/03/2026.
//

#include "window.h"

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
    glfwSetFramebufferSizeCallback(ctx.window, framebuffer_size_callback);
    glfwSetScrollCallback(ctx.window, scroll_wheel_callback);
    glfwSetCursorPosCallback(ctx.window, mouse_callback);
    glfwSetKeyCallback(ctx.window, key_press_callback);

    glfwSetMouseButtonCallback(ctx.window, mouse_button_callback);
    glfwSetCharCallback(ctx.window, nk_glfw3_char_callback);
}

void init_window_context(GLFWwindow* window, struct nk_context* nk_ctx, grid* grid_data, graphics_info* g_info, physics_info* p_info, physics_info* mm_p_info, main_menu_info* mm_info) {
    ctx = (window_context){
        .window = window,

        .m_info = {{0,0}, {0,0}, 1},

        .g_ctx = init_gui_ctx(grid_data, p_info, mm_p_info, g_info, mm_info, nk_ctx),

        .pending_press = -1,
        };
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);

    ctx.g_ctx.g_info->screen_size[0] = width;
    ctx.g_ctx.g_info->screen_size[1] = height;

    glDeleteTextures(1, &ctx.g_ctx.g_info->scene_color_tex.ID);
    glDeleteTextures(1, &ctx.g_ctx.g_info->depth_tex.ID);

    gen_framebuffer_textures(&ctx.g_ctx.g_info->scene_fbo, &ctx.g_ctx.g_info->scene_color_tex, &ctx.g_ctx.g_info->depth_tex, ctx.g_ctx.g_info->screen_size);
}

void process_input(){
    if(glfwGetKey(ctx.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(ctx.window, 1);
}

void scroll_wheel_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera* camera_ptr = &ctx.g_ctx.g_info->g_info3d.cam;

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {

        const float CAMERA_SPEED_FREECAM = 0.2f;
        const float CAMERA_SPEED_ORBIT = 0.05f;
        if(is_orbiting(camera_ptr)) {
            camera_ptr->orbit.orbit_distance -= CAMERA_SPEED_ORBIT * yoffset;
            mouse_movement_orbit(camera_ptr, (ivec3){ctx.g_ctx.grid_data->grid3d_data.size[0],
                                                     ctx.g_ctx.grid_data->grid3d_data.size[1],
                                                     ctx.g_ctx.grid_data->grid3d_data.size[2]},
                                 (vec2){0, 0});
        }
        else
            move_camera_z(camera_ptr, CAMERA_SPEED_FREECAM * yoffset);
    } else {
        nk_gflw3_scroll_callback(window, xoffset, yoffset);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {

    ctx.m_info.pos[0] = xpos; ctx.m_info.pos[1] = ypos;

    if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED){
        return;
    }

    if(ctx.m_info.is_first_mouse) {
        ctx.m_info.last[0] = xpos;
        ctx.m_info.last[1] = ypos;
        ctx.m_info.is_first_mouse = 0;
    }

    float x_offset = xpos - ctx.m_info.last[0];
    float y_offset = ctx.m_info.last[1] - ypos;

    ctx.m_info.last[0] = xpos;
    ctx.m_info.last[1] = ypos;

    camera* camera_ptr = &ctx.g_ctx.g_info->g_info3d.cam;

    if(is_orbiting(camera_ptr)) {
        mouse_movement_orbit(camera_ptr, (ivec3){ctx.g_ctx.grid_data->grid3d_data.size[0],
                                                 ctx.g_ctx.grid_data->grid3d_data.size[1],
                                                 ctx.g_ctx.grid_data->grid3d_data.size[2]},
                             (vec2) {x_offset, y_offset} );
    } else {
        mouse_movement(camera_ptr, (vec2) {x_offset, y_offset} );
    }

}

void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_S:
                toggle_settings(&ctx.g_ctx);
                if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    ctx.m_info.is_first_mouse = 1;
                }
                break;
            case GLFW_KEY_P:
                toggle_paused(&ctx.g_ctx);
                break;
            case GLFW_KEY_O:
                toggle_orbit(&ctx.g_ctx.g_info->g_info3d.cam);
                ctx.m_info.is_first_mouse = 1;
                break;
            case GLFW_KEY_A:
                toggle_auto_orbit(&ctx.g_ctx.g_info->g_info3d.cam);
                break;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    vec2 mouse_pos;
    get_swapped_y_pos(mouse_pos);

    if(action == GLFW_PRESS) {
        ctx.pending_press = handle_click(ctx.g_ctx.mm_info, mouse_pos);
    }

    nk_glfw3_mouse_button_callback(window, button, action, mods);
}

void handle_pending_press() {
    switch(ctx.pending_press) {
        case QUIT:
            glfwSetWindowShouldClose(ctx.window, GL_TRUE);
        case NEW_SIM_ID:
            to_simulation_type(ctx.g_ctx.mm_info);
            break;
        case LOAD_ID:
            break;
        case GRID3D_ID:
            ctx.g_ctx.mm_info->state = MM_OPTIONS;
            init_new_grid_screen3d();
            break;
        case GRID2D_ID:
            ctx.g_ctx.mm_info->state = MM_OPTIONS;
            init_new_grid_screen2d();
            break;
        case BACK:
            to_main_screen(ctx.g_ctx.mm_info);
            toggle_off_grid_creation_draw_settings(&ctx.g_ctx);
            break;
    }
    ctx.pending_press = -1;
}

void handle_main_menu_events() {
    vec2 mouse_pos;
    get_swapped_y_pos(mouse_pos);

    handle_hover(ctx.g_ctx.mm_info, mouse_pos);
}

void init_new_grid_screen3d() {
    init_placeholder_grid3d(ctx.g_ctx.grid_data);
    *ctx.g_ctx.p_info = init_physics_info(ctx.g_ctx.mm_p_info->p_shaders);
    p_info_upload_data(ctx.g_ctx.p_info, ctx.g_ctx.grid_data);

    toggle_on_grid_creation_draw_settings(&ctx.g_ctx);

    ctx.g_ctx.grid_info.initialized = 1;
}

void init_new_grid_screen2d() {
    init_placeholder_grid2d(ctx.g_ctx.grid_data);
    *ctx.g_ctx.p_info = init_physics_info(ctx.g_ctx.mm_p_info->p_shaders);
    p_info_upload_data(ctx.g_ctx.p_info, ctx.g_ctx.grid_data);

    toggle_on_grid_creation_draw_settings(&ctx.g_ctx);

    ctx.g_ctx.grid_info.initialized = 1;
}

void get_swapped_y_pos(vec2 out) {
    out[0] = ctx.m_info.pos[0]; out[1] = ctx.g_ctx.g_info->screen_size[1]-ctx.m_info.pos[1];
}
