cmake_minimum_required(VERSION 3.30)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
    dynamicArray.cpp
    dynamicArray.h
    graph.cpp
    graph.h
    set.cpp
    set.h
    stack.cpp
    stack.h)
