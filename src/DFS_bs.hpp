#ifndef DFS_BS_ALG
#define DFS_BS_ALG

#include <math.h>
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

        int i = 0;
        //TODO: Can this be done smarter?
        int level = (int) (log2(size));
        int res = -100000000000;
        while(i < size){
            std::cout << "Index: " << i << std::endl;
            int val = arr[i];
            if(val == key){
                return val;
            }
            if(arr[i] > key){
                //Go left in tree
                i += 1;
            } else {
                res = val;
                i += pow(2,level);
                //Go right in tree
            }
            level -= 1;
        }

        return res;
    }

}

#endif
