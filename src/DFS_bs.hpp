#ifndef DFS_BS_ALG
#define DFS_BS_ALG

//#include <math.h>
#include "bst.hpp"

using namespace Structure;

namespace dfs {
    const int empty = -1234567;

    const int tab32[32] = {
        0,  9,  1, 10, 13, 21,  2, 29,
        11, 14, 16, 18, 22, 25,  3, 30,
        8, 12, 20, 28, 15, 17, 24,  7,
        19, 27, 23,  6, 26,  5,  4, 31};

    int log2_32 (uint32_t value)
    {
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        return tab32[(uint32_t)(value*0x07C4ACDD) >> 27];
    }

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

//        int level = (int) (log2(size));
        int level = log2_32(size);
        int res = empty;
        int maxSize = (1 << (level + 1)) -1;
        while(i < maxSize && level >= 0){
            int val = arr[i];
            if(val == empty){
                return res;
            }
            if(val != key){
                if(val > key){
                    //Go left in tree
                    i += 1;
                } else {
                    //Go right in tree
                    res = val;
                    i += 1 << level;
                }
            } else {
                return val;
            }
            level -= 1;
        }

        return res;
    }

}

#endif
