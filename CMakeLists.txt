cmake_minimum_required(VERSION 4.2)
project(parallel)

set(CMAKE_CXX_STANDARD 14)

include_directories(task-2)

set(COMMON_SOURCES
        task-2/main.cpp
        task-2/area/area.h)

if(WIN32)
    list(APPEND COMMON_SOURCES task-2/area/area_win/area_win.cpp)
else ()
    list(APPEND COMMON_SOURCES task-2/area/area_posix/area_posix.cpp)
endif ()

add_executable(parallel ${COMMON_SOURCES})

if(NOT WIN32)
    target_link_libraries(parallel pthread)
endif()
