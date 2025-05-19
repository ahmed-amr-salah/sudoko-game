#!/bin/bash

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${BLUE}${BOLD}Rebuilding Sudoku Solver Backend${NC}"

# Check if build directory exists
if [ -d "build" ]; then
    echo -e "${YELLOW}Removing existing build directory...${NC}"
    rm -rf build
    echo -e "${GREEN}Build directory removed.${NC}"
fi

# Create new build directory
echo -e "${BLUE}Creating new build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${BLUE}Configuring with CMake...${NC}"
cmake .. || { echo -e "${RED}CMake configuration failed.${NC}"; exit 1; }

# Build the project
echo -e "${BLUE}Building the project...${NC}"
make || { echo -e "${RED}Build failed.${NC}"; exit 1; }

echo -e "${GREEN}${BOLD}Build completed successfully.${NC}"

# Test the backend on port 8000
echo -e "${BLUE}Testing backend...${NC}"
echo -e "${YELLOW}Attempting to start the backend on port 8000...${NC}"
./sudoku_solver --gui --port 8000 > backend_test.log 2>&1 &
BACKEND_PID=$!

# Wait a moment
sleep 2

# Check if backend is running
if ps -p $BACKEND_PID > /dev/null; then
    echo -e "${GREEN}Backend started successfully on port 8000.${NC}"
    echo -e "${YELLOW}Stopping test instance...${NC}"
    kill -9 $BACKEND_PID 2>/dev/null
    echo -e "${GREEN}Test complete. Backend is working properly on port 8000.${NC}"
    
    # Update the frontend Vite config
    echo -e "${BLUE}Updating frontend Vite configuration to use port 8000...${NC}"
    cd ../frontend
    if [ -f "vite.config.js" ]; then
        # Backup the original file
        cp vite.config.js vite.config.js.bak
        
        # Update all target URLs to use port 8000
        sed -i "s/target: 'http:\/\/localhost:[0-9]\+'/target: 'http:\/\/localhost:8000'/g" vite.config.js
        
        echo -e "${GREEN}Frontend configuration updated to use port 8000.${NC}"
    else
        echo -e "${RED}Could not find vite.config.js in the frontend directory.${NC}"
    fi
    cd ../build
else
    echo -e "${RED}Backend failed to start on port 8000.${NC}"
    echo -e "${YELLOW}Backend output:${NC}"
    cat backend_test.log
    
    # Check for specific errors
    if grep -q "Error binding socket" backend_test.log; then
        echo -e "${RED}Socket binding error detected.${NC}"
        echo -e "${YELLOW}Possible issues:${NC}"
        echo -e "1. The socket implementation in the C++ code might have issues"
        echo -e "2. Your system might have restrictions on binding to port 8000"
        echo -e "3. There might be a lingering socket connection"
        
        echo -e "\n${YELLOW}Let's try with a different port...${NC}"
        ./sudoku_solver --gui --port 9090 > backend_test_alt.log 2>&1 &
        ALT_PID=$!
        sleep 2
        
        if ps -p $ALT_PID > /dev/null; then
            echo -e "${GREEN}Backend started successfully on port 9090.${NC}"
            echo -e "${YELLOW}Stopping test instance...${NC}"
            kill -9 $ALT_PID 2>/dev/null
            echo -e "${GREEN}The backend works on port 9090.${NC}"
            
            # Update the frontend Vite config for port 9090
            echo -e "${BLUE}Updating frontend Vite configuration to use port 9090...${NC}"
            cd ../frontend
            if [ -f "vite.config.js" ]; then
                # Backup the original file
                cp vite.config.js vite.config.js.bak
                
                # Update all target URLs to use port 9090
                sed -i "s/target: 'http:\/\/localhost:[0-9]\+'/target: 'http:\/\/localhost:9090'/g" vite.config.js
                
                echo -e "${GREEN}Frontend configuration updated to use port 9090.${NC}"
                echo -e "${YELLOW}Please update your run_sudoku_solver.sh script to use BACKEND_PORT=9090${NC}"
            else
                echo -e "${RED}Could not find vite.config.js in the frontend directory.${NC}"
            fi
            cd ../build
        else
            echo -e "${RED}Backend also failed on port 9090.${NC}"
            echo -e "${YELLOW}Backend output:${NC}"
            cat backend_test_alt.log
            echo -e "${RED}There might be a deeper issue with the socket implementation.${NC}"
            
            # Let's check if the backend supports the port parameter
            echo -e "${YELLOW}Checking if the backend supports the --port parameter...${NC}"
            ./sudoku_solver --help > help_output.log 2>&1
            if grep -q "port" help_output.log; then
                echo -e "${GREEN}The backend does support the --port parameter.${NC}"
            else
                echo -e "${RED}The backend might not support the --port parameter.${NC}"
                echo -e "${YELLOW}You may need to modify the C++ code to support custom ports.${NC}"
            fi
        fi
    fi
fi

cd ..
echo -e "${BLUE}${BOLD}Rebuild process completed.${NC}" 