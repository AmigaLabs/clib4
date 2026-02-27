#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[]) {
    struct stat statbuf;
    DIR *d;
    struct dirent *dp;
    int dfd, ffd;

    dfd = open("RAM:T", O_RDONLY);
    if (dfd < 0) {
        fprintf(stderr, "Cannot open T: directory\n");
        exit(1);
    }
    if ((d = fdopendir(dfd)) == NULL) {
        fprintf(stderr, "Cannot open T: directory via fd\n");
        exit(1);
    }
    while ((dp = readdir(d)) != NULL) {
        if (dp->d_name[0] == '.')
            continue;
        /* there is a possible race condition here as the file
         * could be renamed between the readdir and the open */

        if ((ffd = openat(dfd, dp->d_name, O_RDONLY)) == -1) {
            perror(dp->d_name);
            continue;
        }

        if (fstat(ffd, &statbuf) == 0) {
            /* found it ... */
            printf("%s: %jdK\n", dp->d_name, (intmax_t)(statbuf.st_size));
        }

        close(ffd);
    }
    closedir(d); // note this implicitly closes dfd
    return 0;
}