cmake_minimum_required(VERSION 3.15)
project(Feyce VERSION 1.0.0 LANGUAGES C)

include_directories(include)

add_executable(feyce_app
    src/feyce.c
)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(feyce_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
