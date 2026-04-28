cmake_minimum_required(VERSION 3.27)
project(SmokeSim VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

add_executable(SmokeSim
        main.c

        common/graphics/glad.c
        common/graphics/graphics_objects.h
        common/graphics/opengl_bindings.h
        common/graphics/graphics.c
        common/graphics/graphics.h

        common/util/shader.c
        common/util/shader.h
        common/util/window.c
        common/util/window.h
        common/util/texture.c
        common/util/texture.h
        common/util/grid_parser.c
        common/util/grid_parser.h

        common/gui/nk_styles.c
        common/gui/nk_styles.h
        common/gui/main_menu.c
        common/gui/main_menu.h
        common/gui/text.c
        common/gui/text.h
        common/gui/gui.c
        common/gui/gui.h
        common/gui/nk_helpers.c
        common/gui/nk_helpers.h

        common/physics/grid.c
        common/physics/grid.h
        common/physics/emitters.c
        common/physics/emitters.h
        common/physics/obstacles.c
        common/physics/obstacles.h
        common/physics/physics.c
        common/physics/physics.h

        3d/camera3d.c
        3d/camera3d.h
        3d/grid3d.c
        3d/grid3d.h
        3d/graphics3d.c
        3d/graphics3d.h
        3d/graphics_objects3d.c
        3d/graphics_objects3d.h
        2d/graphics_objects2d.c
        2d/graphics_objects2d.h

        2d/graphics2d.c
        2d/graphics2d.h
        2d/grid2d.c
        2d/grid2d.h

        nuklear/nuklear_glfw_gl3.h
        nuklear/nuklear.h
        common/util/math_compatibility.h
)

target_include_directories(SmokeSim
        PRIVATE
        ${CMAKE_SOURCE_DIR}/Include
)

target_link_libraries(SmokeSim
        PRIVATE
        OpenGL::GL
        glfw
        Freetype::Freetype
)

if(NOT WIN32)
    target_link_libraries(SmokeSim PRIVATE m)
endif()

if(MSVC)
    target_compile_options(SmokeSim PRIVATE)
else()
    target_compile_options(SmokeSim PRIVATE)
endif()

set_target_properties(SmokeSim PROPERTIES
    OUTPUT_NAME "SmokeSim"
)