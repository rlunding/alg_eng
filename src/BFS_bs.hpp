#ifndef BFS_BS_HPP
#define BFS_BS_HPP

#include <queue>
#include "bst.hpp"

using namespace Structure;

namespace bfs {

    /**
     * Construct an array in BFS-order
     *
     * @param arr
     * @param size
     * @return
     */
    int* build(int* arr, int size) {
        BST* tree = treeFromArray(arr, size);
        Node* root = tree->getRoot();

        int *result = new int[size];
        int i = 0;
        std::queue<Node*> q;
        q.push(root);

        Node* current = NULL;

        while (!q.empty()) {
            current = q.front();
            q.pop();

            result[i] = current->value;
            i++;
            if (current->left) {
                q.push(current->left);
            }
            if (current->right) {
                q.push(current->right);
            }
        }
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
        int best = NULL;
        while (i < size) {
            if (arr[i] > key) {
                i = 2 * i + 1;
            } else if (arr[i] < key) {
                best = arr[i];
                i = 2 * i + 2;
            } else {
                return arr[i];
            }
        }
        return best;
    }

}


#endif
