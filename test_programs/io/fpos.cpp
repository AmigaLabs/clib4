#include <iostream>
#include <sstream>
#include <cwchar>

int main() {
    std::istringstream s("test");
    std::mbstate_t st = s.tellg().state();

    if (std::mbsinit(&st))
        std::cout << "The stream is in the initial shift state\n";
}