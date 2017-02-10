#ifndef DFS_BS_ALG
#define DFS_BS_ALG

#include <math.h>
#include "bst.hpp"

using namespace Structure;

namespace dfs {
    const int empty = -1234567;

    void build_recursive(int* arr, Node* root, int& idx, int level){
        arr[idx] = root->value;
        idx ++;
        if(root->left){
            build_recursive(arr, root->left, idx, level-1);
        } else if(level > 0){
            arr[idx] = root->value;
            idx++;
        }
        if(root->right){
            build_recursive(arr, root->right, idx, level-1);
        } else if(level > 0){
            arr[idx] = root->value;
            idx++;
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
        int levels = (int) (log2(size));

        int arrSize = pow(2, levels + 1) - 1;
        int *result = new int[arrSize];
        int idx = 0;
        build_recursive(result, root, idx, levels);
        delete tree;

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

        int level = (int) (log2(size));
        int res = empty;
        int maxSize = pow(2, level+1) -1;
        while(i < maxSize && level >= 0){
            int val = arr[i];
            if(val == empty){
                return res;
            }
            if(val == key){
                return val;
            }
            if(val > key){
                //Go left in tree
                i += 1;
            } else {
                //Go right in tree
                res = val;
                i += 1 << level;
            }
            level -= 1;
        }

        return res;
    }

}

#endif
