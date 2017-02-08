#ifndef DFS_BS_ALG
#define DFS_BS_ALG

#include "bst.hpp"
using namespace Structure;

namespace dfs {

    void build_recursive(int* arr, Node* root, int& idx){
        arr[idx] = root->value;
        idx += 1;
        if(root->left){
            build_recursive(arr, root->left, idx);
        }
        if(root->right){
            build_recursive(arr, root->right, idx);
        }
    }

    /**
     * Construct an array in DFS-order
     *
     * @param arr
     * @param size
     * @return
     */
    int* build(int* arr, int size) {
        BST* tree = treeFromArray(arr, size);
        Node* root = tree->getRoot();

        int *result = new int[size];
        int idx = 0;
        build_recursive(result, root, idx);

        return result;
    }


    /**
     * Find the predicate of x (denoted key), which is: max {y \in arr | y ≤ x}.
     *
     * @param arr
     * @param size
     * @param key
     * @return
     */
    int pred(int* arr, int size, int key) {
        return key;
    }

}

#endif
