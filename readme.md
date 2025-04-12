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
