#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

int
main(int argc, char **argv) {
    struct timespec times[2];

    if (argc < 2) {
        printf("Usage: touch <filename>\n");
        return 0;
    }
    char *file = argv[1];
    clock_gettime(CLOCK_REALTIME, &times[0]);
    clock_gettime(CLOCK_REALTIME, &times[1]);
    int fd = open( file, O_RDONLY);
    if (fd > 0) {
        int status = futimens(fd, times);
        if (status < 0) {
            printf("Error futimens %s: %d\n", file, errno);
        }
        close(fd);
    }
    return 0;
}