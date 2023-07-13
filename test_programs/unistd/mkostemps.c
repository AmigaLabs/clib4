#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int
main() {
    char template[] = "tempfileXXXXXX";  // Temporary file Template name
    int fd;                              // File descriptor

    fd = mkostemps(template, 0, O_RDWR | O_CREAT);
    if (fd == -1) {
        perror("Error creating temp file");
        return 1;
    }

    printf("Temp file created.\n");

    // Close fd
    close(fd);
    unlink
    return 0;
}