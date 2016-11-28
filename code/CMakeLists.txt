cmake_minimum_required(VERSION 3.0)
project(server)

set(SOURCES
    src/main.cpp
    )

set(HEADERS
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic")

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_BRANCH STREQUAL "master")
    message(STATUS "Release version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Werror")
else()
    message(STATUS "Developer version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -pg")
endif()

add_executable(server ${SOURCES})
