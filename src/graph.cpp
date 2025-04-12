#include "graph.h"
#include <iostream>

// Edge implementation
Edge::Edge(Node* src, Node* dest) : source(src), destination(dest), next(nullptr) {}

Edge::~Edge() {
    next = nullptr;
}

Node* Edge::getSource() const {
    return source;
}

Node* Edge::getDestination() const {
    return destination;
}

Edge* Edge::getNext() const {
    return next;
}

void Edge::setNext(Edge* nextEdge) {
    next = nextEdge;
}

// Node implementation
Node::Node(int nodeId, int r, int c) : id(nodeId), row(r), col(c), value(0), edgeList(nullptr), edgeCount(0) {
    // Initialize domain - all values (1-9) are available initially
    resetDomain();
}

Node::~Node() {
    // Delete all edges in the edge list
    Edge* current = edgeList;
    while (current != nullptr) {
        Edge* next = current->next;
        delete current;
        current = next;
    }
    edgeList = nullptr;
}

int Node::getId() const {
    return id;
}

int Node::getRow() const {
    return row;
}

int Node::getCol() const {
    return col;
}

int Node::getValue() const {
    return value;
}

void Node::setValue(int val) {
    value = val;
}

bool Node::isValueInDomain(int val) const {
    if (val < 1 || val > 9) return false;
    return domain[val-1];
}

void Node::removeFromDomain(int val) {
    if (val >= 1 && val <= 9 && domain[val-1]) {
        domain[val-1] = false;
    }
}

void Node::resetDomain() {
    for (int i = 0; i < 9; i++) {
        domain[i] = true;
    }
}

int Node::getDomainSize() const {
    int size = 0;
    for (int i = 0; i < 9; i++) {
        if (domain[i]) size++;
    }
    return size;
}

void Node::addEdge(Node* neighbor) {
    // Check if edge already exists
    if (isConnectedTo(neighbor)) {
        return;
    }
    
    // Create new edge
    Edge* newEdge = new Edge(this, neighbor);
    
    // Add to the beginning of the edge list
    newEdge->next = edgeList;
    edgeList = newEdge;
    edgeCount++;
}

bool Node::isConnectedTo(Node* node) const {
    Edge* current = edgeList;
    while (current != nullptr) {
        if (current->destination == node) {
            return true;
        }
        current = current->next;
    }
    return false;
}

Edge* Node::getEdgeList() const {
    return edgeList;
}

int Node::getEdgeCount() const {
    return edgeCount;
}

// Graph implementation
Graph::Graph(int initialCapacity) : nodeCount(0), capacity(initialCapacity) {
    nodes = new Node*[capacity];
    for (int i = 0; i < capacity; i++) {
        nodes[i] = nullptr;
    }
}

Graph::~Graph() {
    // Delete all nodes
    for (int i = 0; i < nodeCount; i++) {
        delete nodes[i];
    }
    delete[] nodes;
}

Node* Graph::addNode(int row, int col) {
    if (nodeCount >= capacity) {
        // Resize the array if needed
        int newCapacity = capacity * 2;
        Node** newNodes = new Node*[newCapacity];
        
        for (int i = 0; i < capacity; i++) {
            newNodes[i] = nodes[i];
        }
        
        for (int i = capacity; i < newCapacity; i++) {
            newNodes[i] = nullptr;
        }
        
        delete[] nodes;
        nodes = newNodes;
        capacity = newCapacity;
    }
    
    // Create new node
    Node* newNode = new Node(nodeCount, row, col);
    nodes[nodeCount] = newNode;
    nodeCount++;
    
    return newNode;
}

void Graph::addEdge(Node* source, Node* destination) {
    if (source == nullptr || destination == nullptr) {
        return;
    }
    
    // Add bidirectional edges
    source->addEdge(destination);
    destination->addEdge(source);
}

Node* Graph::getNode(int id) const {
    if (id >= 0 && id < nodeCount) {
        return nodes[id];
    }
    return nullptr;
}

Node* Graph::getNodeByPosition(int row, int col) const {
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i]->getRow() == row && nodes[i]->getCol() == col) {
            return nodes[i];
        }
    }
    return nullptr;
}

int Graph::getNodeCount() const {
    return nodeCount;
}

void Graph::buildSudokuConstraints() {
    // Create 81 nodes for a 9x9 Sudoku grid
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            addNode(row, col);
        }
    }
    
    // Add edges for Sudoku constraints
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            Node* current = getNodeByPosition(row, col);
            
            // Add row constraints
            for (int c = 0; c < 9; c++) {
                if (c != col) {
                    Node* neighbor = getNodeByPosition(row, c);
                    addEdge(current, neighbor);
                }
            }
            
            // Add column constraints
            for (int r = 0; r < 9; r++) {
                if (r != row) {
                    Node* neighbor = getNodeByPosition(r, col);
                    addEdge(current, neighbor);
                }
            }
            
            // Add 3x3 box constraints
            int boxStartRow = (row / 3) * 3;
            int boxStartCol = (col / 3) * 3;
            
            for (int r = boxStartRow; r < boxStartRow + 3; r++) {
                for (int c = boxStartCol; c < boxStartCol + 3; c++) {
                    if (r != row || c != col) {
                        Node* neighbor = getNodeByPosition(r, c);
                        addEdge(current, neighbor);
                    }
                }
            }
        }
    }
}

void Graph::printSudokuGrid() const {
    std::cout << "+-------+-------+-------+" << std::endl;
    
    for (int row = 0; row < 9; row++) {
        std::cout << "| ";
        
        for (int col = 0; col < 9; col++) {
            Node* cell = getNodeByPosition(row, col);
            int value = cell->getValue();
            
            if (value == 0) {
                std::cout << ". ";
            } else {
                std::cout << value << " ";
            }
            
            if (col % 3 == 2) {
                std::cout << "| ";
            }
        }
        
        std::cout << std::endl;
        
        if (row % 3 == 2) {
            std::cout << "+-------+-------+-------+" << std::endl;
        }
    }
}

bool Graph::isValidSudokuValue(Node* node, int value) const {
    if (node == nullptr || value < 1 || value > 9) {
        return false;
    }
    
    // Check if any neighbor has the same value
    Edge* current = node->getEdgeList();
    while (current != nullptr) {
        Node* neighbor = current->getDestination();
        if (neighbor->getValue() == value) {
            return false;
        }
        current = current->next;
    }
    
    return true;
}
