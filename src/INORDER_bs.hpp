#ifndef INORDER_BS_HPP
#define INORDER_BS_HPP

#include "bst.hpp"

using namespace Structure;

namespace inorder {

    /**
     * Construct an array in INORDER-order
     *
     * @param arr
     * @param size
     * @return
     */
    int* build(int* arr, int size) {
        sort(arr, arr + size);
        return arr;
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
        int begin = 0, end = size - 1, middle;
        int best = NULL;
        while (begin < end) {
            middle = begin + ((end - begin) / 2);

            if (arr[middle] > key) { // left
                end = middle;
            } else if(arr[middle] < key) { // Right
                best = arr[middle];
                begin = middle + 1;
            } else {
                return arr[middle];
            }
        }
        middle = begin + ((end - begin) / 2);
        if (arr[middle] <= key) {
            return arr[middle];
        }
        return best;
    }

}


#endif
