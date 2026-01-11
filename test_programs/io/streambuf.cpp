#include <iostream>
#include <fstream>

int main() {
    std::ifstream file("test.c");

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Get the stream buffer from the file
    std::streambuf* pbuf = file.rdbuf();

    // Write the contents of the file directly to std::cout
    std::cout << pbuf;

    return 0;
}
