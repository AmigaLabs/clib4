#include <float.h>
#include <iostream>
#include <chrono>

Float::Float(int a)
    : Test::Test{a}
{
    testCout();
    testPrintf();
    print();
}

void Float::testCout()
{
    clearScreen();
    for (int i = 0; i != amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j != 10000; ++j)
            std::cout << j + 0.12345;
        auto end = std::chrono::high_resolution_clock::now();
        resultsCout.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    }
}

void Float::testPrintf()
{
    clearScreen();
    for (int i = 0; i != amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j!=10000; ++j)
            printf("%.5f", j + 0.12345);
        auto end = std::chrono::high_resolution_clock::now();
        resultsPrintf.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    }
}
