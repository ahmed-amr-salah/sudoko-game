# Sudoku Solver

An interactive Sudoku solver with a modern web-based interface and a high-performance C++ backend. The puzzle is represented as a graph-based constraint satisfaction problem using custom-built data structures without the STL.

---

## 🚀 Getting Started

### Quick Start

The easiest way to run the application is via the provided script:

#### 1. Make the script executable (if needed)

```bash
chmod +x run_sudoku_solver.sh
```

#### 2. Run the application

```bash
./run_sudoku_solver.sh
```

This script will:

1. Check if the required ports are available  
2. Build the C++ backend if needed  
3. Start the frontend server  
4. Start the backend server  
5. Open the application in your default browser  

Once started, access the application at:  
[http://localhost:5173](http://localhost:5173)

---

### 🛠 Manual Setup

If you prefer to set up the application manually:

#### Backend Setup

1. Create a build directory and navigate into it:

```bash
mkdir -p build && cd build
```

2. Configure with CMake:

```bash
cmake ..
```

3. Build the backend:

```bash
make
```

4. Run the backend:

```bash
./sudoku_solver --gui
```

---

## 📘 Sudoku Graph Implementation

### Overview

This project implements a custom graph data structure to model a Sudoku puzzle as a constraint satisfaction problem. All data structures are built from scratch — no STL.

---

### 📁 Files

- `graph.h` – Class declarations for `Edge`, `Node`, and `Graph`  
- `graph.cpp` – Method implementations

---

### 🔧 Structure

- **Edge**: Represents constraints (e.g., shared row, column, or box) between cells  
- **Node**: Represents a Sudoku cell, including its position, value, and possible domain values  
- **Graph**: Manages the overall puzzle, building and enforcing Sudoku rules

---

### ✨ Features

- Graph-based modeling of Sudoku rules (rows, columns, boxes)  
- Domain tracking for each cell  
- Validity checking for value assignments  
- Optional visualization of the Sudoku grid

---

### ▶️ Usage

To use the backend or graph solver independently, compile and run the `sudoku_solver` binary with appropriate flags. The GUI will also utilize this solver internally when launched.

---
