#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main() {
    struct pollfd fds[2];
    int timeout_msecs = 500;

    int p[2]; pipe(p);
    /* Open STREAMS device. */
    fds[0].fd = p[0];
    fds[0].events = POLLIN | POLLOUT;
    fds[1].fd = p[1];
    fds[1].events = POLLOUT | POLLIN;

    int ret = poll(fds, 2, timeout_msecs);
    printf("poll() returned : %d\n", ret);

    if (ret > 0) {
        /* An event on one of the fds has occurred. */
        for (int i = 0; i < 2; i++) {
            if (fds[i].revents & POLLOUT) {
                printf("POLLOUT on %d\n", i);
            }
            if (fds[i].revents & POLLIN) {
                printf("POLLIN on %d\n", i);
            }
        }
    }
    return 0;
}