#include <cmath>
#include <iostream>
#include <cerrno>
#include <cfenv>
#include <cstring>

#pragma STDC FENV_ACCESS ON

int main() {
    std::cout << "asin(1.0) = " << asinl(1) << '\n'
              << "2*asin(1.0) = " << 2 * asinl(1) << '\n'
              << "asin(-0.5) = " << asinl(-0.5) << '\n'
              << "6*asin(-0.5) =" << 6 * asinl(-0.5) << '\n';
    // special values
    std::cout << "asin(0.0) = " << asinl(0) << " asin(-0.0)=" << asin(-0.0) << '\n';
    // error handling
    errno = 0;
    std::feclearexcept(FE_ALL_EXCEPT);
    std::cout << "asin(1.1) = " << asinl(1.1) << '\n';
    if (errno == EDOM)
        std::cout << " errno == EDOM: " << std::strerror(errno) << '\n';
    if (std::fetestexcept(FE_INVALID))
        std::cout << " FE_INVALID raised" << '\n';
}