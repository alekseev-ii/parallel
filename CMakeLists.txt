cmake_minimum_required(VERSION 4.3)
project(parallel)

set(CMAKE_CXX_STANDARD 14)

include_directories(task-2)

add_executable(parallel
        task-2/area.h
        task-2/main.cpp
        task-2/area.cpp)
