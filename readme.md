# Sudoku Solver

An interactive Sudoku solver application with a modern web-based UI and a powerful C++ backend.

## Getting Started

### Quick Start

The easiest way to run the application is using the provided script:

Make the script executable (if needed)
    chmod +x run_sudoku_solver.sh

Run the application
    ./run_sudoku_solver.sh

Create build directory
    mkdir -p build && cd build

Configure with CMake

cmake ..
Build
make

Run the backend

./sudoku_solver --gui

This script will:
1. Check if the required ports are available
2. Build the C++ backend if needed
3. Start the frontend server
4. Start the backend server
5. Open the application in your default browser

Once started, you can access the application at: http://localhost:5173

### Manual Setup

If you prefer to set up the application manually:

#### Backend Setup

# Sudoku Graph Implementation

## Overview
This project implements a custom graph data structure for representing a Sudoku puzzle as a constraint satisfaction problem. All data structures are built from scratch without using the STL library.

## Files
- `graph.h`: Contains class declarations for Edge, Node, and Graph
- `graph.cpp`: Contains the implementation of all methods

## Structure
- **Edge**: Represents connections between Sudoku cells that constrain each other
- **Node**: Represents a single Sudoku cell with position, value, and domain information
- **Graph**: Manages the entire puzzle with methods to build and validate Sudoku constraints

## Features
- Represents Sudoku constraints (row, column, box) as graph edges
- Tracks possible values (domain) for each cell
- Provides methods to check if values are valid for specific cells
- Includes visualization of the Sudoku grid

## Usage
