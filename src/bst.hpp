//
// Created by fhv on 2/1/17.
//

#ifndef ALG_ENG_BST_HPP
#define ALG_ENG_BST_HPP

#include <iostream>
#include <math.h>

using namespace std;

struct Node {
    int value;
    Node *left;
    Node *right;

    Node(int val) {
        this->value = val;
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

private:
    Node *root;
    int size;

    void sortedArrayRecursive(Node *node, int *arr, int &idx){
        if(!node){
            return;
        }
        sortedArrayRecursive(node->left, arr, idx);
        arr[idx] = node->value;
        idx++;
        sortedArrayRecursive(node->right, arr, idx);
    }

    int* helperSortedArray(){
        int* result = new int[size];
        int idx = 0;
        sortedArrayRecursive(root, result, idx);
        return result;
    };


    void addHelper(Node *root, int val) {
        if (root->value > val) {
            if (!root->left) {
                root->left = new Node(val);
            } else {
                addHelper(root->left, val);
            }
        } else {
            if (!root->right) {
                root->right = new Node(val);
            } else {
                addHelper(root->right, val);
            }
        }
    }
//    res = ""
//    if self.right:
//        res += self.right.descriptor(indent + "   ") + newline("got right")
//    res += indent + str(self.value)
//    if self.left:
//        res += newline("left") + self.left.descriptor(indent + "   ")
//    return res
    void printHelper(Node *root, std::string indent) {
        if (!root) return;
        if(root->right){
            printHelper(root->right, indent + string("   "));
        }
        cout << indent << root->value << endl;
        if(root->left){
            printHelper(root->left, indent + "   ");
        }
    }

public:
    BST(Node* root, int size);
    BST();
    ~BST();
    void add(int val) {
        size ++;
        if (root) {
            this->addHelper(root, val);
        } else {
            root = new Node(val);
        }
    }

    int* getSortedArray(){
        return this->helperSortedArray();
    };

    void print() {
        printHelper(this->root, "");
    }
};

BST::BST(){}
BST::BST(Node *root, int size) {
    this->root = root;
    this->size = size;
}

BST::~BST() {
    if(this->root){
        delete this->root;
    }
}

Node* getSubTreeForInterval(int* arr, int start, int end){
    int length = end-start;
    if (length == 2) {
        return new Node(arr[start + 1], new Node(arr[start]), nullptr);
    }
    if (length == 1) {
        return new Node(arr[start], nullptr, nullptr);
    }
    if(length <= 0){
        return nullptr;
    }

    int levels = (int)(log2(length + 1));
    int mid;
    if (length - (pow(2, levels) - 1) > (pow(2,levels-1))){
        mid = pow(2,levels) - 1;
    } else {
        mid = length - (pow(2,levels-1));
    }

    Node* left = getSubTreeForInterval(arr, start, start + mid);
    Node* right = getSubTreeForInterval(arr, start + mid + 1, end);
    return new Node(arr[start + mid], left, right);
}

BST* treeFromArray(int* arr, int size){
    Node* root = getSubTreeForInterval(arr, 0, size);
    return new BST(root, size);
}

#endif //ALG_ENG_BST_HPP
