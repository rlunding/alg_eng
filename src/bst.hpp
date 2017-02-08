#ifndef ALG_ENG_BST_HPP
#define ALG_ENG_BST_HPP

#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;

namespace Structure {

    struct Node {
        int value;
        Node *left;
        Node *right;

        Node(int val) {
            this->value = val;
            this->left = NULL;
            this->right = NULL;
        }

        Node(int val, Node *left, Node *right) {
            this->value = val;
            this->left = left;
            this->right = right;
        }

        ~Node(){
            if(left){ delete left; }
            if(right){ delete right; }
        }
    };

    class BST {
    public:
        BST(Node* root, int size);
        BST();
        ~BST();
        void add(int val);
        Node* getRoot();
        int* getSortedArray();
        void print();
        void print_tree();

    private:
        Node *root;
        int size;

        void sortedArrayRecursive(Node *node, int *arr, int &idx);
        int* helperSortedArray();
        void addHelper(Node *root, int val);
        void printHelper(Node *root, std::string indent);
        string printHelperTree(Node* node, string prefix, bool is_tail);
    };

    BST* treeFromArray(int* arr, int size);

}

#endif //ALG_ENG_BST_HPP
