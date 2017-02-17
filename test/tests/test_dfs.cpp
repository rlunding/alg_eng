#ifndef DFS_TEST
#define DFS_TEST

#include "../lib/catch.hpp"
#include "../../src/DFS_bs.hpp"

TEST_CASE("Simple DFS build test", "[build, DFS]") {
    int size = 3;
    int numbers[] = {3, 2, 1};
    int dfs_layout[] = {2, 1, 3};
    int *result = dfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(dfs_layout[i] == result[i]);
    }
}

TEST_CASE("Simple DFS build test 2", "[build, DFS]") {
    int size = 4;
    int numbers[] = {2,3,4,5};
    int dfs_layout[] = {4, 3, 2, 3, 5, 5, 5};
    int *result = dfs::build(numbers, size);
    for (int i = 0; i < 7; ++i) {
        REQUIRE(dfs_layout[i] == result[i]);
    }
}

TEST_CASE("Larger DFS build test", "[build, DFS]") {
    int size = 26;
    int numbers[] = {26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int dfs_layout[] = {16, 8, 4, 2, 1, 3, 6, 5, 7, 12, 10, 9, 11, 14, 13, 15, 23, 20, 18, 17, 19, 22,
                        21, 22, 25, 24,24, 24, 26, 26,26};
    int *result = dfs::build(numbers, size);
    for (int i = 0; i < 31; ++i) {
        REQUIRE(dfs_layout[i] == result[i]);
    }
}

TEST_CASE("DFS predict simple test - many queries", "[pred, DFS]") {
    int size = 7;
    int numbers[] = {3,6,9,12,15,18,21};
    int *result = dfs::build(numbers, size);

    int test_size = 19;
    int queries[] = {2,         3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int facit[] =   {dfs::empty,3,3,3,6,6,6,9, 9, 9,12,12,12,15,15,15,18,18,18,21,21};

    for(int i = 0; i < test_size; i++){
        int prediction = dfs::pred(result, size, queries[i]);
        REQUIRE(prediction == facit[i]);
    }
    delete result;
}

TEST_CASE("DFS predict bigger tree - many queries", "[pred, DFS]") {
    int size = 15;
    int numbers[] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30};

    int test_size = 19;
    int queries[] = {1,         2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    int facit[] =   {dfs::empty,2,2,4,4,6,6,8,8,10,10,12,12,14,14,16,16,18,18,20,20,22};

    for(int j = 1; j <= size; j++){
        for(int i = 0; i < test_size && i < j; i++){
            int *result = dfs::build(numbers, j);
            int prediction = dfs::pred(result, j, queries[i]);  
            REQUIRE(prediction == facit[i]);
            delete result;
        }
    }
}

#endif
