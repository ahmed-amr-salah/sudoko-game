#include "binaryTree.h"

#include <iostream>
using namespace std;


//node constructor
binaryTree :: node :: node() {
    count = 9;
    left = nullptr;
    right = nullptr;
    for(int i = 0 ; i < count ; i++) {
        possible_num[i] = i + 1;    //initialize the numbers from 1 to 9
    }
}

//remove a possible num by shifting
void binaryTree::node::remove_possible_num(int num) {
    for (int i = 0; i < count; i++) {
        if (possible_num[i] == num) {
            for (int j = i; j < count-1; j++ ) {
                possible_num[j] = possible_num[j+1];
            }
            count--;
            break;
        }
    }
}

//check if a possible num exist
bool binaryTree::node::has_possible_num(int num) const {
    for (int i = 0; i < count; i++)
        if (possible_num[i] == num) return true;
    return false;
}

//get the next possible num (return -1 if empty)
int binaryTree::node::get_next_possible_num() const {
    return (count > 0) ? possible_num[0] : -1;
}

//tree constructor
binaryTree::binaryTree() {
    root = nullptr;
}

//tree destructor
binaryTree::~binaryTree() {
    delete root;
}

//fill tree with initial numbers
void binaryTree::initialize() {
    if(!root) root = new node();
}

//updates the possible nums after adding a num to the tree
void binaryTree::remove(int num) const {
    if(root) root->remove_possible_num(num);
}

//checks if the num is still possible
bool binaryTree::contain(int num) const {
    return root ? root->has_possible_num(num) : false;
}

//returns the next possible num to try
int binaryTree::next_possible_num() const {
    return root ? root->get_next_possible_num() : -1;
}

//check if the cell has any possible nums
bool binaryTree::isEmpty() const {
    return root ? (root->count == 0) : true;
}

//prints the remaining possible nums
void binaryTree::print() const {
    if(!root) {
        cout << "Empty Tree" << endl;
        return;
    }
    cout<<"the remaining possible numbers are: "<<endl;
    for(int i = 0 ; i < root->count ; i++) {
        cout<< root->possible_num[i]<<" ";
    }
}

