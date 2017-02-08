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

TEST_CASE("Larger DFS build test", "[build, DFS]") {
    int size = 26;
    int numbers[] = {26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int dfs_layout[] = {16, 8, 4, 2, 1, 3, 6, 5, 7, 12, 10, 9, 11, 14, 13, 15, 23, 20, 18, 17, 19, 22,
                        21, 25, 24, 26};
    int *result = dfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(dfs_layout[i] == result[i]);
    }
}

TEST_CASE("DFS predict simple test", "[pred, DFS]") {
    int size = 7;
    int numbers[] = {3,6,9,12,15,18,21};
    int *result = dfs::build(numbers, size);

    int prediction = dfs::pred(result, size, 10);
    REQUIRE(prediction == 9);
}

#endif
