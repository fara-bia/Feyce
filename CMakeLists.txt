cmake_minimum_required(VERSION 3.15)
project(Feyce VERSION 1.0.0 LANGUAGES C)

set(APP_NAME "FeyceGUI")

include_directories(include)

find_package(SDL3 REQUIRED)

add_executable(${APP_NAME}
    src/feyce.c
)

target_link_libraries(${APP_NAME} PRIVATE SDL3::SDL3)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${APP_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
