#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/random.h>

int main() {
    unsigned char buffer[10];
    ssize_t bytesRead = getrandom(buffer, sizeof(buffer), 0);

    if (bytesRead == -1) {
        perror("Error reading random data");
        return 1;
    }

    printf("Random data: ");

    for (int i = 0; i < bytesRead; i++) {
        printf("%02x ", buffer[i]);
    }

    printf("\n");

    return 0;
}