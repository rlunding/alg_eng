#ifndef BFS_TEST
#define BFS_TEST

#include "../lib/catch.hpp"

#include "../../src/BFS_bs.hpp"


TEST_CASE("Test very simple build for BFS n = 3", "[BFS build]") {
    int size = 3;
    int numbers[] = {1, 2, 3};
    int bfs_layout[] = {2, 1, 3};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}

TEST_CASE("Test simple build for BFS, n = 5", "[BFS build]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int bfs_layout[] = {4, 2, 5, 1, 3};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}


TEST_CASE("Test simple build for BFS, n = 7", "[BFS build]") {
    int size = 7;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    int bfs_layout[] = {4, 2, 6, 1, 3, 5, 7};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}


TEST_CASE("Test build for BFS", "[BFS build]") {
    int size = 15;
    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    int bfs_layout[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int *result = bfs::build(numbers, size);
    for (int j = 0; j < size; ++j) {
        std::cout << result[j] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}

TEST_CASE("Test simple BFS binary search query", "[BFS pred]") {
    int numbers[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int result = bfs::pred(numbers, 15, 5);
    REQUIRE(result == 5);
}

/*
TEST_CASE("Test BFS binary search query", "[BFS pred]") {
    int numbers[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int result = bfs::pred(numbers, 15, 15);
    REQUIRE(result == 14);
}
 */


#endif
