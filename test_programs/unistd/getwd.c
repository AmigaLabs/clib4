#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifndef MAX_BUF
#define MAX_BUF 200
#endif

int main(void) {
    errno = 0;
    char getcwdbuf[PATH_MAX];

    char *buf = getwd(getcwdbuf);
    if (getwd(getcwdbuf) != getcwdbuf) {
        perror("getwd");
        exit(EXIT_FAILURE);
    }
    printf("Current working directory: %s\n", buf);

    exit(EXIT_SUCCESS);
}