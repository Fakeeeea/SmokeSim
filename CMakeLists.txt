cmake_minimum_required(VERSION 3.27)
project(SmokeSim C)

set(CMAKE_C_STANDARD 11)

find_library(GLFW_LIB NAMES glfw3 PATHS "E:/Libraries/Lib" NO_DEFAULT_PATH)
find_library(FREETYPE_LIB NAMES freetype PATHS "E:/Libraries/Lib" NO_DEFAULT_PATH)

add_executable(SmokeSim
        main.c
        common/graphics/glad.c
        common/util/shader.c
        common/util/shader.h
        3d/graphics3d.c
        3d/graphics3d.h
        3d/camera3d.c
        3d/camera3d.h
        common/physics/grid.c
        common/physics/grid.h
        common/util/window.c
        common/util/window.h
        common/graphics/graphics_objects.h
        common/util/texture.c
        common/util/texture.h
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
        common/graphics/opengl_bindings.h
        common/physics/emitters.c
        common/physics/emitters.h
        common/physics/obstacles.c
        common/physics/obstacles.h
        common/physics/physics.c
        common/physics/physics.h
        common/graphics/graphics.c
        common/graphics/graphics.h
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
        common/util/grid_parser.c
        common/util/grid_parser.h
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