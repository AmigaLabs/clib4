#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    unsigned char buffer[10];
    int result = getentropy(buffer, sizeof(buffer));

    if (result != 0) {
        perror("Error getting entropy");
        return 1;
    }

    printf("Entropy data: ");

    for (int i = 0; i < sizeof(buffer); i++) {
        printf("%02x ", buffer[i]);
    }

    printf("\n");

    return 0;
}