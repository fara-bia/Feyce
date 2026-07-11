cmake_minimum_required(VERSION 3.15...4.4)
project(Feyce VERSION 1.0.0 LANGUAGES C)

set(APP_NAME "FeyceBot")

include_directories(include)

add_executable(${APP_NAME}
    src/feyce.c
)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${APP_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
