#pragma once

#include <string>
#include <iostream>
#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


int* generateArray(int size, int seed=1) {
    if (seed > 0) {
        srand(seed);
    } else {
        srand(time(NULL));
    }

    int *result = new int[size];
    for (int i = 0; i < size; ++i) {
        result[i] = rand() % 10;
    }
    return result;
}
