#include <stdlib.h>
#include <stdio.h>

int main() {
    int a = 1234;
    char buffer[256];
    if (itoa(a, buffer, 10) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", a, 10, buffer);
    }
    int b = -231;
    if (itoa(b, buffer, 10) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", b, 10, buffer);
    }
    int c = 10;
    if (itoa(c, buffer, 2) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", c, 2, buffer);
    }
    return 0;
}