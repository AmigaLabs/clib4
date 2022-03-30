#include <integer.h>
#include <iostream>
#include <chrono>

Integer::Integer(int a)
    : Test::Test{a}
{
    testCout();
    testPrintf();
    print();
}

void Integer::testCout()
{
    clearScreen();
    for (int i=0; i!=amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j=0; j!=10000; ++j)
            std::cout << j;
        auto end = std::chrono::high_resolution_clock::now();
        resultsCout.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
}

void Integer::testPrintf()
{
    clearScreen();
    for (int i = 0; i != amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j != 10000; ++j)
            printf("%d", j);
        auto end = std::chrono::high_resolution_clock::now();
        resultsPrintf.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
}
