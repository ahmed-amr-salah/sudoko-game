cmake_minimum_required(VERSION 3.10)
project(SudokuSolver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/stack.cpp
    src/binaryTree.cpp
    src/dynamicArray.cpp
)

# Add header files
set(HEADERS
    src/graph.h
    src/stack.h
    src/binaryTree.h
    src/dynamicArray.h
)

# Create executable
add_executable(sudoku_solver ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(sudoku_solver PRIVATE src)

# Copy sample Sudoku file to build directory
configure_file(sample_sudoku.txt sample_sudoku.txt COPYONLY)
