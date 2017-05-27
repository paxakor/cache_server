cmake_minimum_required(VERSION 3.0)
project(tests)
add_subdirectory(ut)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic \
    -O0 -ggdb -DDEBUG -pthread")

add_executable(insatiable_client insatiable_client.cpp)
