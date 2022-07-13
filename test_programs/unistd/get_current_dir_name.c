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
    char *buf = get_current_dir_name();
    if (buf == NULL) {
        perror("get_current_dir_name");
        exit(EXIT_FAILURE);
    }
    printf("Current working directory: %s\n", buf);
    free(buf);

    exit(EXIT_SUCCESS);
}