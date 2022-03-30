#include <iostream>
#include <integer.h>
#include <string.h>
#include <float.h>

int main()
{
    std::cout << "Enter amount of test: ";
    int tests;
    std::cin >> tests;
    std::cout << "Select mode:\n"
                 "1. Integer\n"
                 "2. Float\n"
                 "3. String\n";
    int mode;
    std::cin >> mode;

    switch (mode) {
    case 1: {
        Integer test {tests};
        break;
    }
    case 2: {
        Float test {tests};
        break;
    }
    case 3: {
        String test {tests};
        break;
    }
    }
    std::cin.ignore();
    getchar();
    return 0;
}
