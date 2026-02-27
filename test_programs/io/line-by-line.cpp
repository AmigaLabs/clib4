#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream file("test.c");
    std::string line;

    while (std::getline(file, line)) {
        std::cout << line << '\n';
    }

    return 0;
}
