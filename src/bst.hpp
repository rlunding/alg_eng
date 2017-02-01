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

    void printHelper(Node *root) {
        if (!root) return;
        printHelper(root->left);
        cout << root->value << ' ';
        printHelper(root->right);
    }

public:
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
        printHelper(this->root);
    }
};

#endif //ALG_ENG_BST_HPP
