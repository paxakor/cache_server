cmake_minimum_required(VERSION 3.0)
project(tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic \
    -O0 -ggdb -DDEBUG")

add_executable(utils utils_ut.cpp ../../src/utils.cpp ../../src/string_view.cpp)
