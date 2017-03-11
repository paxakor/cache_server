cmake_minimum_required(VERSION 3.0)
project(server)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(ut)

set(SOURCES
    src/args_parser.cpp
    src/client.cpp
    src/log.cpp
    src/main.cpp
    src/parser.cpp
    src/server.cpp
    src/signal.cpp
    src/socket.cpp
    src/string_view.cpp
    src/utils.cpp
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic")



# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_BRANCH STREQUAL "master")
    message(STATUS "Release version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Werror")
else()
    message(STATUS "Developer version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -pg -DDEBUG")
endif()



if(USE_BOOST_FILESYSTEM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_FILESYSTEM")
elseif(USE_EXPSTL_FILESYSTEM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_EXPSTL_FILESYSTEM")
elseif(USE_STL_FILESYSTEM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_STL_FILESYSTEM")
else()
    set(SOURCES ${SOURCES} src/filesystem.cpp)
endif()

add_executable(server ${SOURCES})

if(USE_BOOST_FILESYSTEM)
    target_link_libraries(server boost_system boost_filesystem)
elseif(USE_EXPSTL_FILESYSTEM OR USE_STL_FILESYSTEM)
    target_link_libraries(server stdc++fs)
endif()



file(COPY tests/config.txt DESTINATION ./)
