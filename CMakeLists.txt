cmake_minimum_required(VERSION 3.27)
project(SmokeSim C)

set(CMAKE_C_STANDARD 11)

find_library(GLFW_LIB NAMES glfw3 PATHS "E:/Libraries/Lib" NO_DEFAULT_PATH)
find_library(FREETYPE_LIB NAMES freetype PATHS "E:/Libraries/Lib" NO_DEFAULT_PATH)

add_executable(SmokeSim
        main.c
        common/glad.c
        common/shader.c
        common/shader.h
        3d/graphics3d.c
        3d/graphics3d.h
        3d/camera3d.c
        3d/camera3d.h
        common/grid.c
        common/grid.h
        common/window.c
        common/window.h
        common/graphics_objects.h
        common/texture.c
        common/texture.h
        nuklear/nuklear_glfw_gl3.h
        nuklear/nuklear.h
        common/gui/nk_styles.c
        common/gui/nk_styles.h
        common/gui/main_menu.c
        common/gui/main_menu.h
        common/gui/text.c
        common/gui/text.h
        2d/graphics2d.c
        2d/graphics2d.h
        2d/grid2d.c
        2d/grid2d.h
        3d/grid3d.c
        3d/grid3d.h
        common/opengl_bindings.h
        common/emitters.c
        common/emitters.h
        common/obstacles.c
        common/obstacles.h
        common/physics.c
        common/physics.h
        common/graphics.c
        common/graphics.h
        3d/graphics3d.c
        3d/graphics3d.h
        3d/graphics_objects3d.c
        3d/graphics_objects3d.h
        2d/graphics_objects2d.c
        2d/graphics_objects2d.h
        common/gui/gui.c
        common/gui/gui.h
        common/gui/nk_helpers.c
        common/gui/nk_helpers.h
)

target_include_directories(SmokeSim PRIVATE
    "E:/Libraries/Include"
)

target_link_libraries(SmokeSim
        ${GLFW_LIB}
        ${FREETYPE_LIB}
        opengl32
        user32
        gdi32
        shell32
)

target_compile_definitions(SmokeSim PRIVATE _GLFW_STATIC)