cmake_minimum_required(VERSION 3.6)
project(procgen)

set(CMAKE_CXX_STANDARD 14)


set(COMMON_SOURCES
        refs/gradient_noise/gradient_noise.cpp
        refs/gradient_noise/gradient_noise.hpp
        bezier.hpp
        fcolor.hpp
        helper.hpp
        makefile
        movers.hpp
        rando.hpp
        README.md
        JitterRenderer.cpp JitterRenderer.hpp rando.cpp)

set(PROCGEN_SOURCES
        renderer.cpp )

set(GUI_SOURCES
        refs/imgui/imconfig.h
        refs/imgui/imgui.cpp
        refs/imgui/imgui.h
        refs/imgui/imgui_demo.cpp
        refs/imgui/imgui_draw.cpp
        refs/imgui/imgui_internal.h
        refs/imgui/stb_rect_pack.h
        refs/imgui/stb_textedit.h
        refs/imgui/stb_truetype.h

        refs/gl3w/GL/gl3w.c


        imgui_impl_glfw_gl3.h
        imgui_impl_glfw_gl3.cpp
        gui_main.cpp )


# If you have trouble, it's probably because these libraries aren't installed
include_directories(refs
        /usr/include/libpng12
        /usr/include
        /usr/include/png++
        refs/glfw
        refs/gl3w
        refs/imgui
        )
find_library(LIBPNG png12)
find_library(GLFW glfw3 REQUIRED)
find_library(LGL GL)
find_library(LDL dl)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

add_executable(procgen ${COMMON_SOURCES} ${PROCGEN_SOURCES})
add_executable(gui     ${COMMON_SOURCES} ${GUI_SOURCES})
target_link_libraries(procgen PUBLIC ${LIBPNG})
target_link_libraries(gui PUBLIC ${GLFW})
target_link_libraries(gui PUBLIC ${LDL})
target_link_libraries(gui PUBLIC    ${LGL})