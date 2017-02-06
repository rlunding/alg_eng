#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "../src/data_generator.hpp"
#include "../src/BFS_bs.hpp"
#include "../src/DFS_bs.hpp"

int RUNS = 5;
int MIN_TEST_SIZE = 100;
int MAX_TEST_SIZE = 2000;

struct Algorithm {
    std::string algorithm;
    int (*pred)(int*, int, int); // Array pointer, array size, key --> return pred(x)
    int* (*build)(int*, int); // Array pointer, array size --> return structured array
};

int main() {
    std::cout << "Let's benchmark!" << std::endl;

    std::vector<Algorithm> algorithms;
    algorithms.push_back({"BFS", bfs::pred, bfs::build});
    algorithms.push_back({"DFS", dfs::pred, dfs::build});
    //algorithms.push_back({"INORDER", NULL, NULL});

    std::ofstream resultFile("result.data");
    resultFile << "Algorithm time N" << std::endl;
    for (int i = MIN_TEST_SIZE; i < MAX_TEST_SIZE; i*=2) {
        int* numbers = generateArray(i); //Generate data
        int query = generateQuery();
        for (int j = 0; j < algorithms.size(); ++j) {
            int* arr = algorithms.at(j).build(numbers, i);

            double time = 0;
            for (int k = 0; k < RUNS; ++k) {
                auto start_time = std::chrono::system_clock::now();
                algorithms.at(j).pred(arr, i, query);
                auto end_time = std::chrono::system_clock::now();
                time = (end_time - start_time).count();
            }
            time /= RUNS;

            resultFile << algorithms.at(j).algorithm << " " << time << " " << i << " " << std::endl;

            //delete[] arr;
        }
        //delete[] numbers;
    }
    resultFile.close();

    return 0;
}
