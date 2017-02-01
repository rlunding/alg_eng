#ifndef BFS_TEST
#define BFS_TEST

#include "../lib/catch.hpp"

#include "../../src/BFS_bs.hpp"


TEST_CASE("Test simple BFS binary search query", "[BFS pred]") {
    int numbers[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int result = bfs::pred(numbers, 15, 5);
    REQUIRE(result == 5);
}

TEST_CASE("Test BFS binary search query", "[BFS pred]") {
    int numbers[] = {7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14};
    int result = bfs::pred(numbers, 15, 15);
    REQUIRE(result == 14);
}


#endif
