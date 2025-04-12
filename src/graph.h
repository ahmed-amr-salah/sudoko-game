#ifndef GRAPH_H
#define GRAPH_H

// Forward declaration
class Node;

// Edge class to represent connections between nodes
class Edge {
private:
    Node* source;
    Node* destination;
    Edge* next; // For linked list of edges

public:
    Edge(Node* src, Node* dest);
    ~Edge();
    
    Node* getSource() const;
    Node* getDestination() const;
    Edge* getNext() const;
    void setNext(Edge* nextEdge);
    
    friend class Graph;
    friend class Node;
};

// Node class to represent a cell in the Sudoku puzzle
class Node {
private:
    int id;             // Unique identifier
    int row;            // Row position in Sudoku grid
    int col;            // Column position in Sudoku grid
    int value;          // Current value (0 if unassigned)
    bool domain[9];     // Available values (true if available)
    Edge* edgeList;     // List of edges connecting to other nodes
    int edgeCount;      // Number of edges

public:
    Node(int nodeId, int r, int c);
    ~Node();
    
    int getId() const;
    int getRow() const;
    int getCol() const;
    int getValue() const;
    void setValue(int val);
    
    bool isValueInDomain(int val) const;
    void removeFromDomain(int val);
    void resetDomain();
    int getDomainSize() const;
    
    void addEdge(Node* neighbor);
    bool isConnectedTo(Node* node) const;
    Edge* getEdgeList() const;
    int getEdgeCount() const;
    
    friend class Graph;
};

// Graph class to represent the entire Sudoku puzzle
class Graph {
private:
    Node** nodes;       // Array of node pointers
    int nodeCount;      // Number of nodes
    int capacity;       // Maximum capacity of nodes array

public:
    Graph(int initialCapacity = 81); // Default for 9x9 Sudoku
    ~Graph();
    
    Node* addNode(int row, int col);
    void addEdge(Node* source, Node* destination);
    Node* getNode(int id) const;
    Node* getNodeByPosition(int row, int col) const;
    int getNodeCount() const;
    
    // Utility functions for Sudoku
    void buildSudokuConstraints();
    void printSudokuGrid() const;
    bool isValidSudokuValue(Node* node, int value) const;
};

#endif
