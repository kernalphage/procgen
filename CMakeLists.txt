cmake_minimum_required(VERSION 3.6)
project(procgen)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES
        refs/gradient_noise/gradient_noise.cpp
        refs/gradient_noise/gradient_noise.hpp
        refs/gradient_noise/LICENSE
        refs/gradient_noise/README.md
        bezier.hpp
        fcolor.hpp
        helper.hpp
        makefile
        movers.hpp
        rando.hpp
        README.md
        renderer.cpp
        )

include_directories(refs /usr/include/libpng12 /usr/include/png++)
find_library(LIBPNG png12)


set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_executable(procgen ${SOURCE_FILES})
target_link_libraries(procgen PUBLIC ${LIBPNG})