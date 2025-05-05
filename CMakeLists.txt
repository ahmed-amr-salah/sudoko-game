cmake_minimum_required(VERSION 3.14)
project(SudokuSolver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use FetchContent to download and build dependencies
include(FetchContent)

# Set policy to handle older CMake versions in dependencies
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Fetch nlohmann/json with specific CMake version handling
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2  # Use a specific version
)
# Configure json to use a simpler build approach
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# Alternative approach: use the single header version
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Download the single header file
file(DOWNLOAD
    https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann/json.hpp
    SHOW_PROGRESS
)

# Fetch IXWebSocket
FetchContent_Declare(
    ixwebsocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT_TAG v11.4.3  # Use a specific version
)
# Configure IXWebSocket options
set(USE_TLS OFF CACHE BOOL "")
set(USE_ZLIB OFF CACHE BOOL "")
FetchContent_MakeAvailable(ixwebsocket)

# Add source files
set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/stack.cpp
    src/binaryTree.cpp
    src/sudoku-gui-ipc.cpp
)

# Add header files
set(HEADERS
    src/graph.h
    src/stack.h
    src/binaryTree.h
    src/sudoku-gui-ipc.h
)

# Create executable
add_executable(sudoku_solver ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(sudoku_solver PRIVATE
    nlohmann_json
    ixwebsocket::ixwebsocket
)

# Include directories
target_include_directories(sudoku_solver PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# Copy sample Sudoku files to build directory
configure_file(sample_sudoku_S.txt sample_sudoku_S.txt COPYONLY)
configure_file(sample_sudoku_M.txt sample_sudoku_M.txt COPYONLY)
configure_file(sample_sudoku_H.txt sample_sudoku_H.txt COPYONLY)
