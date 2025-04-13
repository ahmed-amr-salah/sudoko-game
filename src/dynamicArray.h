#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
#include <iostream>
using namespace std;

class dynamicArray {
private:
    int** grid;
    const int SIZE = 9;
public:
    dynamicArray();
    ~dynamicArray();
    void initialize_grid();
    void print_grid();
    bool isSafe(int row, int col, int num);
    bool Solve(); // Will be integrated with the Stack class to solve using backtracking
    bool findEmptyCell(int &row, int &col);

    void fillDiagonalBoxes();
    void fillBox(int row, int col);
    bool fillRemaining(int row, int col);
    void removeDigits(int count);
    int randomGenerator(int min, int max);  //will be integrated with the set
};



#endif
