#include "sudoku-gui-ipc.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SudokuGuiIpc::SudokuGuiIpc(int port) 
    : port_(port), isRunning_(false) {
}

SudokuGuiIpc::~SudokuGuiIpc() {
    stop();
}

bool SudokuGuiIpc::start() {
    if (isRunning_) {
        return true;
    }
    
    // Start the server thread
    serverThread_ = std::thread(&SudokuGuiIpc::serverLoop, this);
    isRunning_ = true;
    
    std::cout << "HTTP server started on port " << port_ << std::endl;
    return true;
}

void SudokuGuiIpc::stop() {
    if (!isRunning_) {
        return;
    }
    
    isRunning_ = false;
    
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    
    std::cout << "HTTP server stopped" << std::endl;
}

void SudokuGuiIpc::sendSudokuGrid(const Graph& sudokuGraph, bool isSolution) {
    std::string jsonStr = sudokuGraphToJson(sudokuGraph, isSolution);
    
    // Add message to queue
    std::lock_guard<std::mutex> lock(mutex_);
    messageQueue_.push(jsonStr);
    
    std::cout << "Sent Sudoku grid: " << (isSolution ? "solution" : "puzzle") << std::endl;
}

void SudokuGuiIpc::sendSolvingStatus(bool success, const std::string& message) {
    json statusJson = {
        {"type", "status"},
        {"success", success},
        {"message", message}
    };
    
    // Add message to queue
    std::lock_guard<std::mutex> lock(mutex_);
    messageQueue_.push(statusJson.dump());
    
    std::cout << "Sent solving status: " << (success ? "success" : "failure") 
              << " - " << message << std::endl;
}

void SudokuGuiIpc::sendError(const std::string& errorMessage) {
    json errorJson = {
        {"type", "error"},
        {"message", errorMessage}
    };
    
    // Add message to queue
    std::lock_guard<std::mutex> lock(mutex_);
    messageQueue_.push(errorJson.dump());
    
    std::cout << "Sent error: " << errorMessage << std::endl;
}

void SudokuGuiIpc::setOnPuzzleReceived(std::function<void(const std::vector<std::vector<int>>& puzzle)> callback) {
    onPuzzleReceived_ = callback;
}

void SudokuGuiIpc::setOnSolveRequested(std::function<void()> callback) {
    onSolveRequested_ = callback;
}

void SudokuGuiIpc::setOnFileUploaded(std::function<void(const std::string& filename, const std::string& content)> callback) {
    onFileUploaded_ = callback;
}

void SudokuGuiIpc::serverLoop() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(serverSocket);
        return;
    }
    
    // Bind socket
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return;
    }
    
    // Listen for connections
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        return;
    }
    
    // Accept connections and handle requests
    while (isRunning_) {
        // Set up select to allow for non-blocking accept
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(serverSocket + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity < 0) {
            if (errno != EINTR) {
                std::cerr << "Error in select" << std::endl;
            }
            continue;
        }
        
        if (activity == 0) {
            // Timeout, check if we should continue running
            continue;
        }
        
        // Accept connection
        int clientSocket;
        struct sockaddr_in clientAddress;
        int addrlen = sizeof(clientAddress);
        
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }
        
        // Read request
        char buffer[4096] = {0};
        int valread = read(clientSocket, buffer, 4096);
        if (valread < 0) {
            std::cerr << "Error reading from socket" << std::endl;
            close(clientSocket);
            continue;
        }
        
        // Handle request
        std::string response = handleRequest(buffer);
        
        // Send response
        send(clientSocket, response.c_str(), response.length(), 0);
        
        // Close connection
        close(clientSocket);
    }
    
    // Close server socket
    close(serverSocket);
}

std::string SudokuGuiIpc::handleRequest(const std::string& request) {
    // Parse HTTP request
    std::istringstream requestStream(request);
    std::string method, path, httpVersion;
    requestStream >> method >> path >> httpVersion;
    
    // Handle different endpoints
    if (path == "/api/puzzle") {
        // Handle POST requests to /api/puzzle
        if (method == "POST") {
            // Find the start of the JSON body
            size_t bodyStart = request.find("\r\n\r\n");
            if (bodyStart != std::string::npos) {
                std::string body = request.substr(bodyStart + 4);
                
                // Debug output to see what's being received
                std::cout << "Received body: " << body << std::endl;
                
                try {
                    // Parse JSON body
                    json requestJson = json::parse(body);
                    
                    // Check request type
                    if (requestJson.contains("type")) {
                        std::string type = requestJson["type"];
                        
                        if (type == "solve" && onSolveRequested_) {
                            // Handle solve request
                            onSolveRequested_();
                            return "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Access-Control-Allow-Origin: *\r\n"
                                   "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                                   "Access-Control-Allow-Headers: Content-Type\r\n"
                                   "\r\n"
                                   "{\"status\":\"solving\"}";
                        }
                        else if (type == "file" && onFileUploaded_) {
                            // Handle file upload
                            if (requestJson.contains("filename") && requestJson.contains("content")) {
                                std::string filename = requestJson["filename"];
                                std::string content = requestJson["content"];
                                
                                // Trim whitespace from content
                                content.erase(0, content.find_first_not_of(" \n\r\t"));
                                content.erase(content.find_last_not_of(" \n\r\t") + 1);
                                
                                onFileUploaded_(filename, content);
                                
                                return "HTTP/1.1 200 OK\r\n"
                                       "Content-Type: application/json\r\n"
                                       "Access-Control-Allow-Origin: *\r\n"
                                       "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                                       "Access-Control-Allow-Headers: Content-Type\r\n"
                                       "\r\n"
                                       "{\"status\":\"file_uploaded\"}";
                            }
                        }
                    }
                }
                catch (const json::exception& e) {
                    // Return detailed error for debugging
                    std::string errorMsg = "{\"error\":\"" + std::string(e.what()) + "\"}";
                    return "HTTP/1.1 400 Bad Request\r\n"
                           "Content-Type: application/json\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                           "Access-Control-Allow-Headers: Content-Type\r\n"
                           "\r\n" + errorMsg;
                }
            }
            
            // If we get here, the request was invalid
            return "HTTP/1.1 400 Bad Request\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                   "Access-Control-Allow-Headers: Content-Type\r\n"
                   "\r\n"
                   "{\"error\":\"Invalid request\"}";
        }
        // Handle OPTIONS requests (CORS preflight)
        else if (method == "OPTIONS") {
            return "HTTP/1.1 204 No Content\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                   "Access-Control-Allow-Headers: Content-Type\r\n"
                   "\r\n";
        }
    } 
    else if (path == "/api/messages" && method == "GET") {
        // Return any pending messages
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (messageQueue_.empty()) {
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[]";
        }
        
        json messages = json::array();
        
        while (!messageQueue_.empty()) {
            messages.push_back(json::parse(messageQueue_.front()));
            messageQueue_.pop();
        }
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + messages.dump();
    } 
    else if (path == "/" && method == "GET") {
        // Serve a simple HTML page for testing
        std::string html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Sudoku Solver</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #333; }
        .grid { display: grid; grid-template-columns: repeat(9, 30px); gap: 1px; margin: 20px 0; }
        .cell { width: 30px; height: 30px; text-align: center; line-height: 30px; border: 1px solid #ccc; }
        button { padding: 10px; background: #4CAF50; color: white; border: none; cursor: pointer; }
    </style>
</head>
<body>
    <h1>Sudoku Solver Test Page</h1>
    <div id="puzzle" class="grid"></div>
    <button id="solve">Solve Puzzle</button>
    <h2>Solution:</h2>
    <div id="solution" class="grid"></div>
    <div id="status"></div>
    
    <script>
        // Initialize empty puzzle
        const puzzle = Array(9).fill().map(() => Array(9).fill(0));
        const solution = Array(9).fill().map(() => Array(9).fill(0));
        
        // Render puzzle grid
        function renderGrid(grid, elementId) {
            const element = document.getElementById(elementId);
            element.innerHTML = '';
            
            for (let row = 0; row < 9; row++) {
                for (let col = 0; col < 9; col++) {
                    const cell = document.createElement('div');
                    cell.className = 'cell';
                    cell.textContent = grid[row][col] || '';
                    element.appendChild(cell);
                }
            }
        }
        
        // Render initial grids
        renderGrid(puzzle, 'puzzle');
        renderGrid(solution, 'solution');
        
        // Handle solve button click
        document.getElementById('solve').addEventListener('click', () => {
            fetch('/api/puzzle', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ type: 'solve' })
            });
            
            document.getElementById('status').textContent = 'Solving...';
            
            // Poll for messages
            pollMessages();
        });
        
        // Poll for messages
        function pollMessages() {
            fetch('/api/messages')
                .then(response => response.json())
                .then(messages => {
                    if (messages.length > 0) {
                        for (const message of messages) {
                            handleMessage(message);
                        }
                    }
                    
                    // Continue polling
                    setTimeout(pollMessages, 1000);
                })
                .catch(error => {
                    console.error('Error polling messages:', error);
                    setTimeout(pollMessages, 5000);
                });
        }
        
        // Handle incoming messages
        function handleMessage(message) {
            if (message.type === 'puzzle') {
                for (let row = 0; row < 9; row++) {
                    for (let col = 0; col < 9; col++) {
                        puzzle[row][col] = message.grid[row][col];
                    }
                }
                renderGrid(puzzle, 'puzzle');
            } else if (message.type === 'solution') {
                for (let row = 0; row < 9; row++) {
                    for (let col = 0; col < 9; col++) {
                        solution[row][col] = message.grid[row][col];
                    }
                }
                renderGrid(solution, 'solution');
            } else if (message.type === 'status') {
                document.getElementById('status').textContent = message.message;
            } else if (message.type === 'error') {
                document.getElementById('status').textContent = 'Error: ' + message.message;
            }
        }
        
        // Start polling for messages
        pollMessages();
    </script>
</body>
</html>
        )";
        
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + html;
    }
    else if (path == "/sample_sudoku_S.txt") {
        std::string content = R"(
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
8 0 0 0 6 0 0 0 3
4 0 0 8 0 3 0 0 1
7 0 0 0 2 0 0 0 6
0 6 0 0 0 0 2 8 0
0 0 0 4 1 9 0 0 5
0 0 0 0 8 0 0 7 9
        )";
        
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/plain\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n" + content;
    }
    else if (path == "/sample_sudoku_M.txt") {
        std::string content = R"(
0 0 0 2 6 0 7 0 1
6 8 0 0 7 0 0 9 0
1 9 0 0 0 4 5 0 0
8 2 0 1 0 0 0 4 0
0 0 4 6 0 2 9 0 0
0 5 0 0 0 3 0 2 8
0 0 9 3 0 0 0 7 4
0 4 0 0 5 0 0 3 6
7 0 3 0 1 8 0 0 0
        )";
        
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/plain\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n" + content;
    }
    else if (path == "/sample_sudoku_H.txt") {
        std::string content = R"(
0 2 0 0 0 0 0 0 0
0 0 0 6 0 0 0 0 3
0 7 4 0 8 0 0 0 0
0 0 0 0 0 3 0 0 2
0 8 0 0 4 0 0 1 0
6 0 0 5 0 0 0 0 0
0 0 0 0 1 0 7 8 0
5 0 0 0 0 9 0 0 0
0 0 0 0 0 0 0 4 0
        )";
        
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/plain\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n" + content;
    }
    
    // Default response for unknown paths
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
}

std::string SudokuGuiIpc::sudokuGraphToJson(const Graph& sudokuGraph, bool isSolution) {
    json gridJson = json::array();
    
    for (int row = 0; row < 9; row++) {
        json rowJson = json::array();
        for (int col = 0; col < 9; col++) {
            Node* cell = sudokuGraph.getNodeByPosition(row, col);
            rowJson.push_back(cell->getValue());
        }
        gridJson.push_back(rowJson);
    }
    
    json result = {
        {"type", isSolution ? "solution" : "puzzle"},
        {"grid", gridJson}
    };
    
    return result.dump();
} 