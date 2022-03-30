#include <test.h>
#include <iostream>

void Test::clearScreen() const
{
    std::cout << std::endl;
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Test::print() const
{
    clearScreen();
    for (int i = 0; i != amount; ++i)
        std::cout << i + 1 << ". cout: " << resultsCout[i] << " ms printf: " << resultsPrintf[i] << " ms\n";

    int avgCout {0};
    int avgPrintf {0};

    for (const int& a : resultsCout)
        avgCout += a;
    for (const int& b : resultsPrintf)
        avgPrintf += b;

    std::cout << "Average: cout: " << avgCout / amount << " ms printf: " << avgPrintf / amount << " ms";
}
