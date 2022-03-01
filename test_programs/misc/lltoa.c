#include <stdlib.h>
#include <stdio.h>

int main() {
    int64_t i;
    char buffer[sizeof(int64_t) * 8 + 1];
    printf("Enter a number: ");
    if (scanf("%lld", &i) == 1) {
        lltoa(i, buffer, 10);
        printf("decimal: %s\n", buffer);
        lltoa(i, buffer, 16);
        printf("hexadecimal: %s\n", buffer);
        lltoa(i, buffer, 8);
        printf("octal: %s\n", buffer);
    }
    return 0;
}