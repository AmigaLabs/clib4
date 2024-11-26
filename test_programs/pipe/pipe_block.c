#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {

    int fds[2];
    int result = -1;

    if (pipe(fds) == -1) {
        printf("Failed to generate pipe.\n");
        return -1;
    }

    if (fcntl(fds[0], F_SETFD, FD_CLOEXEC) == -1 ||
        fcntl(fds[1], F_SETFD, FD_CLOEXEC) == -1) {
        printf("Failed to set FD_CLOEXEC flag.\n");
        goto cleanup;
    }

    int fd = fds[1];
    int r;
    do
        r = fcntl(fd, F_GETFL);
    while (r == -1 && errno == EINTR);

    if (r == -1) {
        printf("Call to F_GETFL failed.\n");
        goto cleanup;
    }

    int set = 1;
    int flags;
    if (set)
        flags = r | O_NONBLOCK;
    else
        flags = r & ~O_NONBLOCK;

    do
        r = fcntl(fd, F_SETFL, flags);
    while (r == -1 && errno == EINTR);

    if (r) {
        printf("Call to F_SETFL failed.\n");
        goto cleanup;
    }

    printf("Success. Cleaning up.\n");
    result = 0;

    cleanup:
    close(fds[0]);
    close(fds[1]);

    return result;
}