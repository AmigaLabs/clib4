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

    if (utimensat(AT_FDCWD, file, times, 0) < 0) {
        printf("Error utimensat %s\n", file);
    }

    return 0;
}