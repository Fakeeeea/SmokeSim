// TODO: WORLD CENTER GRID CENTER;
// TODO: CLEANUP CODE (PERMANENT TODO) MAIN.C MESS
// TODO: ADD TO GRAPHICS SETTING VELOCITY ARROWS SETTINGS; VORTICITY ARROW SETTINGS; OBSTACLES SETTINGS;
// TODO: STYLE NUKLEAR
// TODO: MAIN MENU
// TODO: ADD TO SETTINGS V CYCLE PASSES

#include <glad/glad.h>

#include "common/util/shader.h"
#include "common/graphics/graphics.h"
#include "common/physics/physics.h"
#include "common/util/window.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#define NK_MAX_FLOAT_PRECISION 5

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:5287)
#pragma warning(disable:4116)
#endif

#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

grid menu_grid;
physics_info menu_p_info;

physics_shaders p_shaders;

grid sim_grid;
graphics_info g_info;
physics_info p_info;
main_menu_info mm_info;

//TODO: BEFORE SHIPPING REVERT MAX SIZE TO 2 ^ 16 BEFORE SOMEONE'S COMPUTER CRASHES

int main() {

    init_glfw();
    GLFWwindow* window = create_window();
    if(window == NULL) return -1;

    glfwMakeContextCurrent(window);
    if(!load_glad()) return -1;

    struct nk_glfw glfw = {0};
    struct nk_context *nk_ctx = nk_glfw3_init(&glfw, window, 0);
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);

    //struct nk_font *font = nk_font_atlas_add_default(atlas, 24, NULL);

    nk_glfw3_font_stash_end(&glfw);

    //nk_style_set_font(nk_ctx, &font->handle);

    p_shaders.shaders2d = compile_physics_shaders2d();
    p_shaders.shaders3d = compile_physics_shaders3d();
    g_info = init_graphics_info();

    init_main_menu_grid3d(&menu_grid, get_default_p_settings().t_ambient);
    menu_p_info = get_mm_p_info(p_shaders, &menu_grid);

    init_main_menu(&mm_info, (ivec2) {INIT_SCREEN_SIZE_X, INIT_SCREEN_SIZE_Y});
    init_window_context(window, nk_ctx, &sim_grid, &g_info, &p_info, &menu_p_info, &mm_info);
    setup_callbacks();

    bind_physics_buffers(&menu_grid);
    init_solid_map(&menu_grid, &menu_p_info.p_shaders);

    while(!glfwWindowShouldClose(window)) {
        process_input();
        nk_glfw3_new_frame(&glfw);

        if(is_orbiting(&g_info.g_info3d.cam)) {
            if(mm_info.state == MM_MAIN_SCREEN || mm_info.state == MM_SIMULATION_TYPE)
                auto_orbit(&g_info.g_info3d.cam, menu_grid.grid3d_data.size, (float) glfwGetTime() * 0.3f);
            else if(ctx.g_ctx.grid_info.initialized == 1)
                auto_orbit(&g_info.g_info3d.cam, sim_grid.grid3d_data.size, (float) glfwGetTime() * 0.3f);
        }

        if(mm_info.state != MM_CLOSED)
            get_viewport_data_mm((ivec2){g_info.screen_size[0], g_info.screen_size[1]}, g_info.draw_area);
        else
            draw_area_reset(&g_info);

        if(mm_info.state == MM_MAIN_SCREEN || mm_info.state == MM_SIMULATION_TYPE)
            bind_physics_buffers(&menu_grid);
        else if(ctx.g_ctx.grid_info.created)
            bind_physics_buffers(&sim_grid);

        if(mm_info.state == MM_MAIN_SCREEN || mm_info.state == MM_SIMULATION_TYPE) {
            run_physics_step(&menu_grid, &menu_p_info);
        } else if (ctx.g_ctx.grid_info.created == 1 && !p_info.paused){
            run_physics_step(&sim_grid, &p_info);
        }

        if(mm_info.state == MM_MAIN_SCREEN || mm_info.state == MM_SIMULATION_TYPE) {
            draw_step(&menu_grid, &g_info, &menu_p_info);
        } else if(ctx.g_ctx.grid_info.initialized == 1){
            draw_step(&sim_grid, &g_info, &p_info);
        }

        viewport_reset(&g_info);

        handle_main_menu_events();
        draw_gui(&ctx.g_ctx);

        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        if(mm_info.state == MM_MAIN_SCREEN || mm_info.state == MM_SIMULATION_TYPE)
            update_time(menu_p_info.time_ubo, glfwGetTime());
        else if(ctx.g_ctx.grid_info.created == 1)
            update_time(p_info.time_ubo, glfwGetTime());

        handle_pending_press();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_p_info(&menu_p_info);
    free_grid(&menu_grid);
    nk_glfw3_shutdown(&glfw);
    free_main_menu(&mm_info);
    glfwTerminate();
    return 0;
}