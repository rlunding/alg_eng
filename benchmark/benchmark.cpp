#include <iostream>
#include <vector>
#include "../src/number_generator.hpp"


int RUNS = 5;
int TEST_SIZE = 20;

struct Algorithm {
    std::string algorithm;
    int (*pred)(int*, int, int); // Array pointer, array size, key --> return pred(x)
    int* (*build)(int*, int); // Array pointer, array size --> return structured array
};

int main() {
    std::cout << "Let's benchmark!" << std::endl;

    std::vector<Algorithm> algorithms;
    //algorithms.push_back({"BFS", NULL, NULL});
    //algorithms.push_back({"DFS", NULL, NULL});
    //algorithms.push_back({"INORDER", NULL, NULL});

    for (int i = 0; i < TEST_SIZE; i+=10) {
        int* numbers = generateArray(i); //Generate data
        int query = generateQuery();
        for (int j = 0; j < algorithms.size(); ++j) {
            int* arr = algorithms.at(j).build(numbers, i);

            for (int k = 0; k < RUNS; ++k) {
                algorithms.at(j).pred(arr, i, query);
            }
            delete[] arr;
        }
        delete[] numbers;
    }

    return 0;
}
