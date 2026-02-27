#include <stdio.h>
#define _GNU_SOURCE
#include <strings.h>  // For ffsll function

int main() {
    long long int num = 0b00010000;  // Binary representation of the number

    int position = ffsll(num);

    if (position == 0) {
        printf("No bits are set in the number.\n");
    } else {
        printf("The first bit set is at position: %d\n", position);
    }

    return 0;
}
