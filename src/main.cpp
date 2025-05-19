#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "graph.h"
#include "stack.h"
#include "binaryTree.h"
#include "sudoku-gui-ipc.h"

// Function to read Sudoku puzzle from file
bool readSudokuFromFile(const std::string& filename, Graph& sudokuGraph) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    // Build the graph structure for Sudoku
    sudokuGraph.buildSudokuConstraints();
    
    std::string line;
    int row = 0;
    
    while (std::getline(inputFile, line) && row < 9) {
        int col = 0;
        for (size_t i = 0; i < line.length() && col < 9; i++) {
            if (line[i] == ',' || line[i] == ' ') {
                continue; // Skip commas and spaces
            }
            
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            if (cell == nullptr) {
                std::cerr << "Error: Invalid cell position (" << row << "," << col << ")" << std::endl;
                return false;
            }
            
            if (line[i] >= '1' && line[i] <= '9') {
                int value = line[i] - '0';
                cell->setValue(value);
            } else if (line[i] == '*' || line[i] == '0' || line[i] == '.') {
                cell->setValue(0); // Empty cell
            } else {
                std::cerr << "Error: Invalid character in input file: " << line[i] << std::endl;
                return false;
            }
            
            col++;
        }
        
        if (col < 9) {
            std::cerr << "Error: Row " << row << " has fewer than 9 columns" << std::endl;
            return false;
        }
        
        row++;
    }
    
    if (row < 9) {
        std::cerr << "Error: Input file has fewer than 9 rows" << std::endl;
        return false;
    }
    
    inputFile.close();
    return true;
}

// Function to solve the Sudoku puzzle using backtracking
bool solveSudoku(Graph& sudokuGraph) {
    Stack moveStack;
    
    // Find first empty cell
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            if (cell->getValue() == 0) {
                // Try values 1-9 for this cell
                for (int val = 1; val <= 9; val++) {
                    if (sudokuGraph.isValidSudokuValue(cell, val)) {
                        // Place this value and push the move onto the stack
                        cell->setValue(val);
                        moveStack.push(Move(row, col, val));
                        goto nextCell; // Move to the next empty cell
                    }
                }
                
                // If we get here, no valid value was found for this cell
                // We need to backtrack
                while (!moveStack.isEmpty()) {
                    Move lastMove = moveStack.pop();
                    Node* lastCell = sudokuGraph.getNodeByPosition(lastMove.row, lastMove.col);
                    
                    // Try the next value for the last cell
                    bool foundNextValue = false;
                    for (int val = lastMove.value + 1; val <= 9; val++) {
                        if (sudokuGraph.isValidSudokuValue(lastCell, val)) {
                            lastCell->setValue(val);
                            moveStack.push(Move(lastMove.row, lastMove.col, val));
                            foundNextValue = true;
                            break;
                        }
                    }
                    
                    if (foundNextValue) {
                        // Continue solving from this point
                        row = lastMove.row;
                        col = lastMove.col;
                        goto nextCell;
                    } else {
                        // No valid value found, continue backtracking
                        lastCell->setValue(0);
                    }
                }
                
                // If we've exhausted all possibilities, the puzzle is unsolvable
                return false;
            }
            
            nextCell:;
        }
    }
    
    return true;
}

// Alternative solver using recursion and the binary tree for domain tracking
bool solveWithBinaryTree(Graph& sudokuGraph) {
    // Find an empty cell
    int emptyRow = -1, emptyCol = -1;
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            if (cell->getValue() == 0) {
                emptyRow = row;
                emptyCol = col;
                break;
            }
        }
        if (emptyRow != -1) break;
    }
    
    // If no empty cell is found, the puzzle is solved
    if (emptyRow == -1) return true;
    
    // Get the empty cell
    Node* emptyCell = sudokuGraph.getNodeByPosition(emptyRow, emptyCol);
    
    // Create a binary tree to track possible values
    binaryTree domain;
    domain.initialize();
    
    // Remove values that are already used by neighbors
    Edge* edge = emptyCell->getEdgeList();
    while (edge != nullptr) {
        Node* neighbor = edge->getDestination();
        int neighborValue = neighbor->getValue();
        if (neighborValue != 0) {
            domain.remove(neighborValue);
        }
        edge = edge->getNext();
    }
    
    // Try each possible value
    while (!domain.isEmpty()) {
        int value = domain.next_possible_num();
        domain.remove(value);
        
        if (sudokuGraph.isValidSudokuValue(emptyCell, value)) {
            emptyCell->setValue(value);
            
            // Recursively solve the rest of the puzzle
            if (solveWithBinaryTree(sudokuGraph)) {
                return true;
            }
            
            // If we get here, this value didn't work
            emptyCell->setValue(0);
        }
    }
    
    // No solution found with any value
    return false;
}

// New function to handle file content directly (for web uploads)
bool readSudokuFromString(const std::string& content, Graph& sudokuGraph) {
    std::istringstream inputStream(content);
    
    // Build the graph structure for Sudoku
    sudokuGraph.buildSudokuConstraints();
    
    std::string line;
    int row = 0;
    
    while (std::getline(inputStream, line) && row < 9) {
        int col = 0;
        for (size_t i = 0; i < line.length() && col < 9; i++) {
            if (line[i] == ',' || line[i] == ' ') {
                continue; // Skip commas and spaces
            }
            
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            if (cell == nullptr) {
                std::cerr << "Error: Invalid cell position (" << row << "," << col << ")" << std::endl;
                return false;
            }
            
            if (line[i] >= '1' && line[i] <= '9') {
                int value = line[i] - '0';
                cell->setValue(value);
            } else if (line[i] == '*' || line[i] == '0' || line[i] == '.') {
                cell->setValue(0); // Empty cell
            } else {
                std::cerr << "Error: Invalid character in input: " << line[i] << std::endl;
                return false;
            }
            
            col++;
        }
        
        if (col < 9) {
            std::cerr << "Error: Row " << row << " has fewer than 9 columns" << std::endl;
            return false;
        }
        
        row++;
    }
    
    if (row < 9) {
        std::cerr << "Error: Input has fewer than 9 rows" << std::endl;
        return false;
    }
    
    return true;
}

// New function to load Sudoku from a 2D vector (for direct GUI input)
bool loadSudokuFromGrid(const std::vector<std::vector<int>>& grid, Graph& sudokuGraph) {
    if (grid.size() != 9) {
        std::cerr << "Error: Grid must have 9 rows" << std::endl;
        return false;
    }
    
    // Build the graph structure for Sudoku
    sudokuGraph.buildSudokuConstraints();
    
    for (int row = 0; row < 9; row++) {
        if (grid[row].size() != 9) {
            std::cerr << "Error: Row " << row << " must have 9 columns" << std::endl;
            return false;
        }
        
        for (int col = 0; col < 9; col++) {
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            if (cell == nullptr) {
                std::cerr << "Error: Invalid cell position (" << row << "," << col << ")" << std::endl;
                return false;
            }
            
            int value = grid[row][col];
            if (value < 0 || value > 9) {
                std::cerr << "Error: Invalid value " << value << " at position (" << row << "," << col << ")" << std::endl;
                return false;
            }
            
            cell->setValue(value);
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    // Check if we should run in GUI mode or CLI mode
    bool guiMode = false;
    std::string inputFile;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--gui" || arg == "-g") {
            guiMode = true;
        } else if (!arg.empty() && arg[0] != '-') {
            inputFile = arg;
        }
    }
    
    if (guiMode) {
        // GUI mode with WebSocket server
        SudokuGuiIpc ipc(3000);
        Graph sudokuGraph;
        bool puzzleLoaded = false;
        
        // Set up callbacks
        ipc.setOnFileUploaded([&](const std::string& filename, const std::string& content) {
            std::cout << "Received file: " << filename << std::endl;
            
            // Reset the graph
            sudokuGraph = Graph();
            
            // Load the puzzle from the file content
            if (readSudokuFromString(content, sudokuGraph)) {
                puzzleLoaded = true;
                std::cout << "Puzzle loaded successfully" << std::endl;
                
                // Send the initial grid to the frontend
                ipc.sendSudokuGrid(sudokuGraph);
            } else {
                ipc.sendError("Failed to load puzzle from file");
            }
        });
        
        ipc.setOnPuzzleReceived([&](const std::vector<std::vector<int>>& puzzle) {
            std::cout << "Received puzzle from GUI" << std::endl;
            
            // Reset the graph
            sudokuGraph = Graph();
            
            // Load the puzzle from the grid
            if (loadSudokuFromGrid(puzzle, sudokuGraph)) {
                puzzleLoaded = true;
                std::cout << "Puzzle loaded successfully" << std::endl;
                
                // Send the initial grid to the frontend
                ipc.sendSudokuGrid(sudokuGraph);
            } else {
                ipc.sendError("Failed to load puzzle from GUI");
            }
        });
        
        ipc.setOnSolveRequested([&]() {
            std::cout << "Solve requested" << std::endl;
            
            if (!puzzleLoaded) {
                ipc.sendError("No puzzle loaded");
                return;
            }
            
            // Create a copy of the original puzzle for display
            Graph originalGraph = sudokuGraph;
            
            // Solve the puzzle
            std::cout << "Solving..." << std::endl;
            bool solved = solveSudoku(sudokuGraph);
            
            if (solved) {
                std::cout << "Puzzle solved successfully" << std::endl;
                ipc.sendSolvingStatus(true, "Puzzle solved successfully");
                ipc.sendSudokuGrid(sudokuGraph, true);
            } else {
                std::cout << "No solution exists for this puzzle" << std::endl;
                ipc.sendSolvingStatus(false, "No solution exists for this puzzle");
            }
        });
        
        // Start the WebSocket server
        if (!ipc.start()) {
            std::cerr << "Failed to start WebSocket server" << std::endl;
            return 1;
        }
        
        // If an input file was provided, load it
        if (!inputFile.empty()) {
            if (readSudokuFromFile(inputFile, sudokuGraph)) {
                puzzleLoaded = true;
                std::cout << "Puzzle loaded from file: " << inputFile << std::endl;
                ipc.sendSudokuGrid(sudokuGraph);
            } else {
                ipc.sendError("Failed to load puzzle from file: " + inputFile);
            }
        }
        
        std::cout << "Sudoku Solver GUI server running. Press Enter to exit." << std::endl;
        std::cin.get();
        
        ipc.stop();
    } else {
        // CLI mode (original behavior)
        if (inputFile.empty() && argc > 1) {
            inputFile = argv[1];
        }
        
        if (inputFile.empty()) {
            std::cerr << "Usage: " << argv[0] << " <input_file> [--gui]" << std::endl;
            return 1;
        }
        
        Graph sudokuGraph;
        
        // Read the Sudoku puzzle from file
        if (!readSudokuFromFile(inputFile, sudokuGraph)) {
            std::cerr << "Failed to read Sudoku puzzle from file." << std::endl;
            return 1;
        }
        
        std::cout << "Initial Sudoku puzzle:" << std::endl;
        sudokuGraph.printSudokuGrid();
        
        // Solve the puzzle
        std::cout << "\nSolving..." << std::endl;
        bool solved = solveSudoku(sudokuGraph);
        
        if (solved) {
            std::cout << "\nSolved Sudoku puzzle:" << std::endl;
            sudokuGraph.printSudokuGrid();
        } else {
            std::cout << "\nNo solution exists for this Sudoku puzzle." << std::endl;
        }
    }
    
    return 0;
}
