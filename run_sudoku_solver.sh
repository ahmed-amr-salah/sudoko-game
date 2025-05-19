#!/bin/bash

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# ASCII Art Banner
echo -e "${PURPLE}"
echo "  _____           _       _                _____       _                 "
echo " / ____|         | |     | |              / ____|     | |                "
echo "| (___  _   _  __| | ___ | | ___   _     | (___   ___ | |_   _____ _ __ "
echo " \___ \| | | |/ _\` |/ _ \| |/ / | | |     \___ \ / _ \| \ \ / / _ \ '__|"
echo " ____) | |_| | (_| | (_) |   <| |_| |     ____) | (_) | |\ V /  __/ |   "
echo "|_____/ \__,_|\__,_|\___/|_|\_\\__,_|    |_____/ \___/|_| \_/ \___|_|   "
echo -e "${NC}"
echo -e "${BOLD}${CYAN}Interactive Sudoku Solver with Modern UI${NC}\n"

# Configuration
BACKEND_PORT=3000
FRONTEND_PORT=5173
BACKEND_DIR="./build"
FRONTEND_DIR="./frontend"
BACKEND_EXECUTABLE="sudoku_solver"

# Function to check if a port is in use
check_port() {
    local port=$1
    local process=$(lsof -i :$port -t 2>/dev/null)
    if [ -n "$process" ]; then
        echo -e "${YELLOW}Port $port is in use by process ID: $process${NC}"
        return 0
    else
        return 1
    fi
}

# Function to kill a process using a specific port
kill_process_on_port() {
    local port=$1
    local process=$(lsof -i :$port -t 2>/dev/null)
    if [ -n "$process" ]; then
        echo -e "${YELLOW}Killing process $process on port $port...${NC}"
        kill -9 $process 2>/dev/null
        sleep 1
        echo -e "${GREEN}Process terminated.${NC}"
    fi
}

# Check if build directory exists
if [ ! -d "$BACKEND_DIR" ]; then
    echo -e "${RED}Error: Build directory not found.${NC}"
    echo -e "${YELLOW}Creating build directory and building the project...${NC}"
    mkdir -p build
    cd build
    cmake ..
    make
    cd ..
    
    if [ ! -f "$BACKEND_DIR/$BACKEND_EXECUTABLE" ]; then
        echo -e "${RED}Error: Failed to build the backend executable.${NC}"
        exit 1
    fi
fi

# Check if frontend directory exists
if [ ! -d "$FRONTEND_DIR" ]; then
    echo -e "${RED}Error: Frontend directory not found.${NC}"
    exit 1
fi

# Check if backend executable exists
if [ ! -f "$BACKEND_DIR/$BACKEND_EXECUTABLE" ]; then
    echo -e "${RED}Error: Backend executable not found.${NC}"
    echo -e "${YELLOW}Building the project...${NC}"
    cd build
    make
    cd ..
    
    if [ ! -f "$BACKEND_DIR/$BACKEND_EXECUTABLE" ]; then
        echo -e "${RED}Error: Failed to build the backend executable.${NC}"
        exit 1
    fi
fi

# Check and free backend port if needed
echo -e "${BLUE}Checking if backend port $BACKEND_PORT is available...${NC}"
if check_port $BACKEND_PORT; then
    echo -e "${YELLOW}Port $BACKEND_PORT is in use. Attempting to free it...${NC}"
    kill_process_on_port $BACKEND_PORT
    sleep 1
    
    if check_port $BACKEND_PORT; then
        echo -e "${RED}Failed to free port $BACKEND_PORT. Please close the application using this port manually.${NC}"
        exit 1
    fi
fi
echo -e "${GREEN}Backend port $BACKEND_PORT is available.${NC}"

# Check and free frontend port if needed
echo -e "${BLUE}Checking if frontend port $FRONTEND_PORT is available...${NC}"
if check_port $FRONTEND_PORT; then
    echo -e "${YELLOW}Port $FRONTEND_PORT is in use. Attempting to free it...${NC}"
    kill_process_on_port $FRONTEND_PORT
    sleep 1
    
    if check_port $FRONTEND_PORT; then
        echo -e "${RED}Failed to free port $FRONTEND_PORT. Please close the application using this port manually.${NC}"
        exit 1
    fi
fi
echo -e "${GREEN}Frontend port $FRONTEND_PORT is available.${NC}"

# Start the frontend first
echo -e "${BLUE}Starting the frontend server on port $FRONTEND_PORT...${NC}"
cd $FRONTEND_DIR

# Check if npm is installed
if ! command -v npm &> /dev/null; then
    echo -e "${RED}Error: npm is not installed. Please install Node.js and npm.${NC}"
    exit 1
fi

# Start the frontend server
npm run dev &
FRONTEND_PID=$!
cd ..

# Wait for frontend to initialize
echo -e "${YELLOW}Waiting for frontend to initialize...${NC}"
sleep 3

# Check if frontend started successfully
if ! ps -p $FRONTEND_PID > /dev/null; then
    echo -e "${RED}Error: Frontend failed to start.${NC}"
    exit 1
fi

echo -e "${GREEN}Frontend started successfully with PID: $FRONTEND_PID${NC}"

# Now start the backend
echo -e "${BLUE}Starting the backend server on port $BACKEND_PORT...${NC}"
cd $BACKEND_DIR

# Start the backend in a way that ignores the Enter key
# We'll use 'cat' to consume stdin so the backend doesn't receive Enter keypresses
cat | ./$BACKEND_EXECUTABLE --gui > backend_output.log 2>&1 &
BACKEND_PID=$!
cd ..

# Wait for backend to initialize
echo -e "${YELLOW}Waiting for backend to initialize...${NC}"
sleep 2

# Check if backend started successfully
if ! ps -p $BACKEND_PID > /dev/null; then
    echo -e "${RED}Error: Backend process terminated unexpectedly.${NC}"
    echo -e "${YELLOW}Backend output:${NC}"
    cat $BACKEND_DIR/backend_output.log
    kill -9 $FRONTEND_PID 2>/dev/null
    exit 1
fi

# Check for successful startup message
if grep -q "HTTP server started on port" $BACKEND_DIR/backend_output.log; then
    echo -e "${GREEN}Backend started successfully with PID: $BACKEND_PID${NC}"
    echo -e "${YELLOW}Note: The backend is designed to stop when Enter is pressed.${NC}"
    echo -e "${YELLOW}The script is preventing this by redirecting stdin.${NC}"
else
    # Check for binding error in the log
    if grep -q "Error binding socket" $BACKEND_DIR/backend_output.log; then
        echo -e "${RED}Error: Backend failed to bind to port $BACKEND_PORT.${NC}"
        echo -e "${YELLOW}This could be because:${NC}"
        echo -e "  1. Another process is using the port (even though our check didn't detect it)"
        echo -e "  2. The system is restricting socket binding (permissions issue)"
        echo -e "  3. The network interface is not available"
        echo -e "${YELLOW}Trying with a different port...${NC}"
        
        # Kill the failed backend process
        kill -9 $BACKEND_PID 2>/dev/null
        
        # Try with a different port
        BACKEND_PORT=8081
        echo -e "${BLUE}Attempting to start backend on port $BACKEND_PORT...${NC}"
        cd $BACKEND_DIR
        cat | ./$BACKEND_EXECUTABLE --gui --port $BACKEND_PORT > backend_output.log 2>&1 &
        BACKEND_PID=$!
        cd ..
        
        sleep 2
        
        if ! ps -p $BACKEND_PID > /dev/null || ! grep -q "HTTP server started on port" $BACKEND_DIR/backend_output.log; then
            echo -e "${RED}Error: Backend failed to start on alternate port as well.${NC}"
            echo -e "${YELLOW}Backend output:${NC}"
            cat $BACKEND_DIR/backend_output.log
            kill -9 $FRONTEND_PID 2>/dev/null
            exit 1
        fi
        
        # Update the frontend proxy configuration for the new port
        echo -e "${YELLOW}Updating frontend proxy configuration for port $BACKEND_PORT...${NC}"
        cd $FRONTEND_DIR
        sed -i "s/target: 'http:\/\/localhost:[0-9]\+'/target: 'http:\/\/localhost:$BACKEND_PORT'/g" vite.config.js
        # Restart the frontend
        kill -9 $FRONTEND_PID 2>/dev/null
        npm run dev &
        FRONTEND_PID=$!
        cd ..
        sleep 3
    else
        echo -e "${RED}Error: Backend may have started but with unexpected output.${NC}"
        echo -e "${YELLOW}Backend output:${NC}"
        cat $BACKEND_DIR/backend_output.log
        echo -e "${YELLOW}Continuing anyway since the process is still running...${NC}"
    fi
fi

# Display access information
echo -e "\n${BOLD}${CYAN}Sudoku Solver is now running!${NC}"
echo -e "${BOLD}Access the application at:${NC} ${GREEN}http://localhost:$FRONTEND_PORT${NC}"
echo -e "${BOLD}Backend API is available at:${NC} ${GREEN}http://localhost:$BACKEND_PORT/api${NC}"
echo -e "\n${YELLOW}Press Ctrl+C to stop both servers${NC}\n"

# Handle script termination
trap "echo -e '\n${YELLOW}Shutting down servers...${NC}'; kill -9 $BACKEND_PID 2>/dev/null; kill -9 $FRONTEND_PID 2>/dev/null; echo -e '${GREEN}Servers stopped.${NC}'; exit 0" SIGINT SIGTERM

# Keep the script running
wait $FRONTEND_PID
# If frontend exits, kill the backend too
kill -9 $BACKEND_PID 2>/dev/null
echo -e "${YELLOW}All servers stopped.${NC}" 