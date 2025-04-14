cmake_minimum_required(VERSION 3.29)
project(sudoko_game)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(sudoko_game
    src/binaryTree.cpp
    src/binaryTree.h
    src/dynamicArray.cpp
    src/dynamicArray.h
    src/graph.cpp
    src/graph.h
    src/stack.cpp
    src/stack.h)
