#pragma once

#include <string>
#include <iostream>
#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int MIN = 0;
int MAX = 10;

int generateQuery(int min=MIN, int max=MAX) {
    return MIN + (rand() % (int)(MAX - MIN + 1));
}

int* generateArray(int size, int seed=1) {
    if (seed > 0) {
        srand(seed);
    } else {
        srand(time(NULL));
    }

    int *result = new int[size];
    for (int i = 0; i < size; ++i) {
        result[i] = generateQuery();
    }
    for (int j = 0; j < size; ++j) {
        std::cout << result[j] << ", ";
    }
    return result;
}

