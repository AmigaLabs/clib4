/* Compile with -ansi or -std=c90 */
#include <features.h>

inline int test() { return 3; }

int main() {
    test();
    return 0;
}