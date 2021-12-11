cmake_minimum_required(VERSION 3.18)

project(AoC2021 LANGUAGES CXX)

add_compile_options(-Wall -Werror -Wpedantic -std=gnu++23)

file(GLOB QS src/q*.cc)
file(GLOB inputs ${CMAKE_CURRENT_SOURCE_DIR}/input/*)
add_executable(AoC2021
    ${QS}
    ${inputs}
    answers
    main.cc
    test_runner.h
    src/util.h
)
