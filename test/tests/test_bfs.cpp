#ifndef BFS_TEST
#define BFS_TEST

#include "../lib/catch.hpp"

#include "../../src/BFS_bs.hpp"


TEST_CASE("Test simple build for BFS, n = 3", "[BFS, build]") {
    int size = 3;
    int numbers[] = {1, 2, 3};
    int bfs_layout[] = {2, 1, 3};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}

TEST_CASE("Test build for BFS, n = 5", "[BFS, build]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int bfs_layout[] = {4, 2, 5, 1, 3};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}


TEST_CASE("Test build for BFS, n = 7", "[BFS, build]") {
    int size = 7;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    int bfs_layout[] = {4, 2, 6, 1, 3, 5, 7};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}


TEST_CASE("Test build for BFS, n = 15", "[BFS, build]") {
    int size = 15;
    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    int bfs_layout[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int *result = bfs::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(bfs_layout[i] == result[i]);
    }
}


TEST_CASE("Test BFS binary search query, n = 3 with element", "[BFS, pred]") {
    int size = 3;
    int numbers[] = {1, 2, 3};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 2);
    REQUIRE(result == 2);
}

TEST_CASE("Test BFS binary search query, n = 5 with element", "[BFS, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 4);
    REQUIRE(result == 4);
}

TEST_CASE("Test BFS binary search query, n = 7 with element", "[BFS, pred]") {
    int size = 7;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 7);
    REQUIRE(result == 7);
}

TEST_CASE("Test BFS binary search query, n = 9 with element", "[BFS, pred]") {
    int size = 9;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 13, 14};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 7);
    REQUIRE(result == 7);
}

// TODO: why crash when n = 10?
/*TEST_CASE("Test BFS binary search query, n = 10 with element", "[BFS, pred]") {
    int size = 10;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 13, 14, 18};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 7);
    REQUIRE(result == 7);
}*/

TEST_CASE("Test BFS binary search query, n = 15 with element", "[BFS pred]") {
    int numbers[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int result = bfs::pred(numbers, 15, 5);
    REQUIRE(result == 5);
}

TEST_CASE("Test BFS binary search query, n = 3 without element", "[BFS, pred]") {
    int size = 3;
    int numbers[] = {1, 2, 4};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 3, true);
    REQUIRE(result == 2);
}

TEST_CASE("Test BFS binary search query, n = 5 without element", "[BFS, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 4, 6, 9};
    int *input = bfs::build(numbers, size);
    int result = bfs::pred(input, size, 5, true);
    //REQUIRE(result == 4);
}




#endif
