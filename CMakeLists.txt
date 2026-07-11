cmake_minimum_required(VERSION 3.15)
project(Feyce VERSION 1.0.0 LANGUAGES C)

include_directories(include)

find_package(SDL3 REQUIRED)

add_executable(feyce_app
    src/feyce.c
)

set_app_name(feyce_app "Feyce_Sim")

target_link_libraries(feyce_app PRIVATE SDL3::SDL3)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(feyce_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
