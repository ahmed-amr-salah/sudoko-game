//cpp #include "stack.h"
#include <iostream>

using namespace std;

// Move implementation
Move::Move(int r, int c, int v) : row(r), col(c), value(v) {}

// Stack implementation
Stack::Stack(int initialCapacity) : topIndex(-1), capacity(initialCapacity) {
    data = new Move[capacity];
}

Stack::~Stack() {
    delete[] data;
    data = nullptr;
}

bool Stack::isEmpty() const {
    return topIndex == -1;
}

int Stack::size() const {
    return topIndex + 1;
}

void Stack::resize() {
    int newCapacity = capacity * 2;
    Move* newData = new Move[newCapacity];

    for (int i = 0; i < capacity; i++) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
    capacity = newCapacity;
}

void Stack::push(const Move& move) {
    if (topIndex + 1 >= capacity) {
        resize();
    }
    data[++topIndex] = move;
}

Move Stack::pop() {
    if (isEmpty()) {
        cerr << "Stack underflow!" << endl;
        exit(1);
    }
    return data[topIndex--];
}

Move Stack::top() const {
    if (isEmpty()) {
        cerr << "Stack is empty!" << endl;
        exit(1);
    }
    return data[topIndex];
}