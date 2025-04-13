#include "dynamicArray.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

dynamicArray::dynamicArray() {
    grid = new int*[SIZE];
    for (int i = 0; i < SIZE; ++i)
        grid[i] = new int[SIZE];
    initialize_grid();
}

dynamicArray::~dynamicArray() {
    for (int i = 0; i < SIZE; ++i)
        delete[] grid[i];
    delete[] grid;
}

void dynamicArray::initialize_grid() {
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            grid[i][j] = 0;
}

void dynamicArray::print_grid() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j)
            std::cout << grid[i][j] << " ";
        std::cout << std::endl;
    }
}

bool dynamicArray::isSafe(int row, int col, int num) {
    for (int x = 0; x < SIZE; x++)
        if (grid[row][x] == num || grid[x][col] == num)
            return false;

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (grid[i + startRow][j + startCol] == num)
                return false;

    return true;
}

bool dynamicArray::findEmptyCell(int &row, int &col) {
    for (row = 0; row < SIZE; row++) {
        for (col = 0; col < SIZE; col++) {
            if (grid[row][col] == 0)
                return true;
        }
    }
    return false;
}

bool dynamicArray::Solve() {

}

int dynamicArray::randomGenerator(int min, int max) {

}

void dynamicArray::fillBox(int row, int col) {

}

void dynamicArray::fillDiagonalBoxes() {
    for (int i = 0; i < SIZE; i += 3)
        fillBox(i, i);
}

bool dynamicArray::fillRemaining(int i, int j) {
    if (j >= SIZE && i < SIZE - 1) {
        i++;
        j = 0;
    }
    if (i >= SIZE && j >= SIZE)
        return true;

    if (i < 3) {
        if (j < 3) j = 3;
    } else if (i < 6) {
        if (j == (i / 3) * 3) j += 3;
    } else {
        if (j == 6) {
            i++; j = 0;
            if (i >= SIZE) return true;
        }
    }

    for (int num = 1; num <= SIZE; num++) {
        if (isSafe(i, j, num)) {
            grid[i][j] = num;
            if (fillRemaining(i, j + 1)) return true;
            grid[i][j] = 0;
        }
    }
    return false;
}

void dynamicArray::removeDigits(int count) {
    while (count != 0) {
        int i = randomGenerator(0, SIZE - 1);
        int j = randomGenerator(0, SIZE - 1);
        if (grid[i][j] != 0) {
            grid[i][j] = 0;
            count--;
        }
    }
}
