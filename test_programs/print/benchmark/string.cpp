#include <string.h>
#include <iostream>
#include <chrono>

String::String(int a)
    : Test::Test{a}
{
    testCout();
    testPrintf();
    print();
}

void String::testCout()
{
    clearScreen();

    for (int i = 0; i != amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j != 10000; ++j)
            std::cout << "abcdef";
        auto end = std::chrono::high_resolution_clock::now();
        resultsCout.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
}

void String::testPrintf()
{
    clearScreen();

    for (int i = 0; i != amount; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j != 10000; ++j)
            printf("%s","abcdef");
        auto end = std::chrono::high_resolution_clock::now();
        resultsPrintf.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
}
