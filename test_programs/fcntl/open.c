#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {

    int fd1 = open("T:", O_PATH);
    if (fd1 == -1) {
        perror("open");
        return 1;
    }

    int fd2 = openat(fd1, "usb.log", O_RDONLY);
    if (fd2 == -1) {
        perror("openat");
        close(fd1);
        return 1;
    }
    // do fancy things with fd2, now opened
    // with access mode read-only

    close(fd1);
    close(fd2);


    return 0;
}