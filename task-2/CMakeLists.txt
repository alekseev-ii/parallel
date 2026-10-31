cmake_minimum_required(VERSION 4.2)
project(T2)

set(CMAKE_CXX_STANDARD 14)

set(COMMON_SOURCES
        main.cpp
        area/area.h)

if (WIN32)
    list(APPEND COMMON_SOURCES area/area_win/area_win.cpp)
else ()
    list(APPEND COMMON_SOURCES area/area_posix/area_posix.cpp)
endif ()

add_executable(T2 ${COMMON_SOURCES})

target_include_directories(T2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT WIN32)
    target_link_libraries(T2 pthread)
endif ()
