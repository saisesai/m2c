cmake_minimum_required(VERSION 3.21)

project(m2c)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(PNG REQUIRED)
find_package(argparse REQUIRED)
find_package(Freetype REQUIRED)

add_executable(
        m2c
        main.cc
        image.cc
        font.cc
)
target_link_libraries(
        m2c PRIVATE
        PNG::PNG
        argparse::argparse
        Freetype::Freetype
)
target_compile_definitions(
        m2c PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
)