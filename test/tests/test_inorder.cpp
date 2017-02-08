#ifndef INORDER_TEST
#define INORDER_TEST

#include "../lib/catch.hpp"

#include "../../src/INORDER_bs.hpp"


TEST_CASE("Test simple build for INORDER, n = 3", "[INORDER, build]") {
    int size = 3;
    int numbers[] = {1, 2, 3};
    int inorder_layout[] = {1, 2, 3};
    int *result = inorder::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(inorder_layout[i] == result[i]);
    }
}

TEST_CASE("Test build for INORDER, n = 5", "[INORDER, build]") {
    int size = 5;
    int numbers[] = {4, 2, 5, 1, 3};
    int inorder_layout[] = {1, 2, 3, 4, 5};
    int *result = inorder::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(inorder_layout[i] == result[i]);
    }
}


TEST_CASE("Test build for INORDER, n = 7", "[INORDER, build]") {
    int size = 7;
    int numbers[] = {4, 2, 6, 1, 3, 5, 7};
    int inorder_layout[] = {1, 2, 3, 4, 5, 6, 7};
    int *result = inorder::build(numbers, size);
    for (int i = 0; i < size; ++i) {
        REQUIRE(inorder_layout[i] == result[i]);
    }
}



TEST_CASE("Test INORDER binary search query, n = 3 with element", "[INORDER, pred]") {
    int size = 3;
    int numbers[] = {1, 2, 3};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 2);
    REQUIRE(result == 2);
}

TEST_CASE("Test INORDER binary search query, n = 5 with element", "[INORDER, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 4);
    REQUIRE(result == 4);
}

TEST_CASE("Test INORDER binary search query, n = 7 with element", "[INORDER, pred]") {
    int size = 7;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 7);
    REQUIRE(result == 7);
}

TEST_CASE("Test INORDER binary search query, n = 9 with element", "[INORDER, pred]") {
    int size = 9;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 13, 14};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 7);
    REQUIRE(result == 7);
}

TEST_CASE("Test INORDER binary search query, n = 10 with element", "[INORDER, pred]") {
    int size = 10;
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 13, 15};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 7);
    REQUIRE(result == 7);
}

TEST_CASE("Test INORDER binary search query, n = 3 without element", "[INORDER, pred]") {
    int size = 3;
    int numbers[] = {1, 2, 4};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 3);
    REQUIRE(result == 2);
}

TEST_CASE("Test INORDER binary search query, n = 5 without element", "[INORDER, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 4, 6, 9};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 5);
    REQUIRE(result == 4);
}

TEST_CASE("Test INORDER binary search query, n = 5 without smaller element", "[INORDER, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 0);
    REQUIRE(result == NULL);
}

TEST_CASE("Test INORDER binary search query, n = 10 without smaller element", "[INORDER, pred]") {
    int size = 10;
    int numbers[] = {12, 3, 1, 4, 5, 6, 8, 3, 2, 1};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, -10);
    REQUIRE(result == NULL);
}

TEST_CASE("Test INORDER binary search query, n = 5 with only smaller elements", "[INORDER, pred]") {
    int size = 5;
    int numbers[] = {1, 2, 3, 4, 5};
    int *input = inorder::build(numbers, size);
    int result = inorder::pred(input, size, 10);
    REQUIRE(result == 5);
}


#endif
