#ifndef BINARYTREE_H
#define BINARYTREE_H



class binaryTree {
private:
    struct node {
        int possible_num [9]{};  //possible numbers
        int count;             //count of valid numbers
        node *left;
        node *right;

        node();
        void remove_possible_num(int num);
        bool has_possible_num(int num) const;
        int get_next_possible_num() const;
    };

    node *root;

public:
    binaryTree();
    ~binaryTree();

    void initialize(); //fill tree with initial numbers
    void remove(int num) const; //updates the possible nums after adding a num to the tree
    bool contain(int num) const; //checks if the num is still possible
    bool isEmpty() const; //check if the cell has any possible nums
    int next_possible_num() const; //returns the next possible num to try
    void print() const; //prints the remaining possible nums

};



#endif

