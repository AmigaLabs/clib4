#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int
main(void) {

    /* Open MUST fail */
    int fd1 = open("T:usb.log", O_DIRECTORY);
    if (fd1 < 0) {
        perror("Error on open");
        return 1;
    }

    close(fd1);

    return 0;
}