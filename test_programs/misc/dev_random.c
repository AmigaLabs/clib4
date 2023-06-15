#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    enableUnixPaths();

    int fd = open("/dev/random", O_RDONLY);

    if (fd == -1) {
        perror("Error opening /dev/random");
        return 1;
    }

    unsigned char buffer[10];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    if (bytesRead == -1) {
        perror("Error reading from /dev/random");
        close(fd);
        return 1;
    }

    printf("Random data: ");

    for (int i = 0; i < bytesRead; i++) {
        printf("%02x ", buffer[i]);
    }

    printf("\n");

    close(fd);

    return 0;
}