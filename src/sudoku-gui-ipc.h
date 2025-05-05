#ifndef SUDOKU_GUI_IPC_H
#define SUDOKU_GUI_IPC_H

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include "graph.h"

class SudokuGuiIpc {
public:
    SudokuGuiIpc(int port = 8080);
    ~SudokuGuiIpc();

    // Start the HTTP server
    bool start();
    
    // Stop the HTTP server
    void stop();
    
    // Send the current Sudoku grid to connected clients
    void sendSudokuGrid(const Graph& sudokuGraph, bool isSolution = false);
    
    // Send solving status to connected clients
    void sendSolvingStatus(bool success, const std::string& message = "");
    
    // Send error message to connected clients
    void sendError(const std::string& errorMessage);
    
    // Set callback for when a new Sudoku puzzle is received from the frontend
    void setOnPuzzleReceived(std::function<void(const std::vector<std::vector<int>>& puzzle)> callback);
    
    // Set callback for when a solve request is received
    void setOnSolveRequested(std::function<void()> callback);
    
    // Set callback for when a file upload is received
    void setOnFileUploaded(std::function<void(const std::string& filename, const std::string& content)> callback);

private:
    // Port to listen on
    int port_;
    
    // Flag to track if server is running
    bool isRunning_;
    
    // Server thread
    std::thread serverThread_;
    
    // Mutex for thread safety
    std::mutex mutex_;
    
    // Queue of messages to send to clients
    std::queue<std::string> messageQueue_;
    
    // Callbacks
    std::function<void(const std::vector<std::vector<int>>& puzzle)> onPuzzleReceived_;
    std::function<void()> onSolveRequested_;
    std::function<void(const std::string& filename, const std::string& content)> onFileUploaded_;
    
    // Server thread function
    void serverLoop();
    
    // Handle HTTP requests
    std::string handleRequest(const std::string& request);
    
    // Convert Sudoku graph to JSON string
    std::string sudokuGraphToJson(const Graph& sudokuGraph, bool isSolution);
};

#endif // SUDOKU_GUI_IPC_H 