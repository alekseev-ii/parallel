cmake_minimum_required(VERSION 4.3)
project(parallel)

set(CMAKE_CXX_STANDARD 14)

include_directories(task-2)

set(COMMON_SOURCES
        task-2/main.cpp
        task-2/area/area.h
        task-2/area/area_posix/area_posix.cpp)

if(WIN32)
    list(APPEND COMMON_SOURCES task-2/area/area_win/area_win.cpp)
endif ()

add_executable(parallel ${COMMON_SOURCES})
