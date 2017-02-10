#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "../src/data_generator.hpp"
#include "../src/BFS_bs.hpp"
#include "../src/DFS_bs.hpp"
#include "../src/INORDER_bs.hpp"

int RUNS = 10;
int MIN_TEST_SIZE = 1000000;
int MAX_TEST_SIZE = 5000000;

struct Algorithm {
    std::string algorithm;
    int (*pred)(int*, int, int); // Array pointer, array size, key --> return pred(x)
    int* (*build)(int*, int); // Array pointer, array size --> return structured array
};


void test_running_time() {
    std::cout << "test_running_time!" << std::endl;
    std::vector<Algorithm> algorithms;
    algorithms.push_back({"BFS", bfs::pred, bfs::build});
    //algorithms.push_back({"DFS", dfs::pred, dfs::build});
    algorithms.push_back({"INORDER", inorder::pred, inorder::build});

    // Open file and print header
    std::ofstream resultFile("test_running_time.data");
    for (int i = 0; i < algorithms.size(); ++i) {
        resultFile << algorithms.at(i).algorithm << " ";
    }
    resultFile << "N" << std::endl;

    // start test
    for (int i = MIN_TEST_SIZE; i < MAX_TEST_SIZE; i*=2) {
        int* numbers = generateArray(i); //Generate data
        int query = generateQuery();
        double *times = new double[algorithms.size()];

        for (int j = 0; j < algorithms.size(); ++j) {
            int* arr = algorithms.at(j).build(numbers, i);

            double time = 0;
            for (int k = 0; k < RUNS; ++k) {
                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
                algorithms.at(j).pred(arr, i, query);
                std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
                time += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            }
            time /= (double) RUNS;
            times[j] = time;
        }
        for (int k = 0; k < algorithms.size(); ++k) {
            resultFile << times[k] << " ";
            std::cout << times[k] << " ";
        }
        resultFile << i << std::endl;
        std::cout << i << std::endl;

    }
    resultFile.close();
    // generate plot file
    std::ofstream plotfile("test_running_time.gnuplot");
    plotfile << "set term png\n"
        "set output 'test_running_time.png'\n"
        "set ylabel 'time [s]' rotate by 90\n"
        "set xlabel 'n [# elements]'\n"
        "set key autotitle columnhead\n"
        "set title 'Comparing Pred(x) running time'\n"
        "set key left top\n"
        "plot for [col=1:"+std::to_string(algorithms.size())+"] 'test_running_time.data' using "+std::to_string(algorithms.size()+1)+":col with lines" << std::endl;
    plotfile.close();
    system("gnuplot test_running_time.gnuplot");
    system("rm test_running_time.gnuplot");
}

int main() {
    std::cout << "Let's benchmark!" << std::endl;
    test_running_time();
    return 0;
}
