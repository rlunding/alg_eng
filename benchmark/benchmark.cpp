#include <iostream>


struct Algorithm {
    std::string algorithm;
    int (*pred)(int*, int, int); // Array pointer, array size, key --> return pred(x)
    int* (*build)(int*, int); // Array pointer, array size --> return structured array
};

int main() {
    std::cout << "Let's benchmark!" << std::endl;
    return 0;
}
