# Copyright (c) 2026 fara-bia
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15...4.3)
project(Feyce VERSION 1.0.0 LANGUAGES C)

set(APP_NAME "FeyceBot")

include_directories(include)

add_executable(${APP_NAME}
    src/feyce.c
    src/decode.c
    src/square_rep.c
    src/evaluate.c
    src/calc_movs.c
)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${APP_NAME} PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()
