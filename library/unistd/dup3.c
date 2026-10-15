/*
 * dup3() differs from dup2() in two ways: it fails when both descriptors are
 * equal, and it accepts O_CLOEXEC. clib4 has no atomic close-on-exec dup, so the
 * flag is set with fcntl() after dup2().
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <errno.h>
#include <fcntl.h>

int
dup3(int oldfd, int newfd, int flags) {
    int saved_errno;

    if (oldfd == newfd || (flags & ~O_CLOEXEC) != 0) {
        errno = EINVAL;
        return -1;
    }

    if (dup2(oldfd, newfd) < 0)
        return -1;

    if (flags & O_CLOEXEC) {
        if (fcntl(newfd, F_SETFD, FD_CLOEXEC) < 0) {
            saved_errno = errno;
            close(newfd);
            errno = saved_errno;
            return -1;
        }
    }

    return newfd;
}
