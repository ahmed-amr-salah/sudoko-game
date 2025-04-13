//header #ifndef STACK_H
#define STACK_H


struct Move {
    int row;
    int col;
    int value;

    Move(int r = 0, int c = 0, int v = 0);
};

// Stack for backtracking Sudoku moves using dynamic array
class Stack {
private:
    Move* data;
    int topIndex;
    int capacity;

    void resize();

public:
    Stack(int initialCapacity = 10);
    ~Stack();

    void push(const Move& move);
    Move pop();
    Move top() const;
    bool isEmpty() const;
    int size() const;
};

#endif // STACK_H