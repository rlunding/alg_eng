#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "bst.hpp"
#include "bbst.hpp"

template<typename T>
int binary_search(const std::vector<T> &vec, int start, int end, const T &key) {
    // Termination condition: start index greater than end index
    if (start > end) return -1;

    // Find the middle element of the vector and use that for splitting
    // the array into two pieces.
    const int middle = start + ((end - start) / 2);

    if (vec[middle] == key) {
        return middle;
    } else if (vec[middle] > key) {
        return binary_search(vec, start, middle - 1, key);
    } else {
        return binary_search(vec, middle + 1, end, key);
    }
}

template<typename Iterator, typename T>
Iterator binary_search(Iterator &begin, Iterator &end, const T &key) {
    const Iterator NotFound = end;

    while (begin < end) {
        const Iterator middle = begin + (std::distance(begin, end) / 2);

        if (*middle == key) {
            return middle;
        } else if (*middle > key) {
            end = middle;
        } else {
            begin = middle + 1;
        }
    }
}


int main() {
    int search = 6;
    int myints[] = {1, 2, 3, 4, 5, 4, 3, 2, 1};
    std::vector<int> v(myints, myints + 9);
    std::sort(v.begin(), v.end());

    std::cout << "Looking for a " << search << "... ";
    if (binary_search(v, 0, v.size() - 1, search))
        std::cout << "Found!" << std::endl;
    else std::cout << "not found." << std::endl;

    int array1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};

    BST* tree = treeFromArray(array1, 19);
    tree->print();
    delete tree;

    return 0;
}
