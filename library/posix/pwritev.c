/*
 * pwritev() is built on writev() and lseek(), which clib4 provides. It moves the
 * file offset, writes, and restores the offset. Other users of the same
 * descriptor can observe the temporary offset.
 */

#ifndef _UIO_HEADERS_H
#include "uio_headers.h"
#endif /* _UIO_HEADERS_H */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

ssize_t
pwritev(int file_descriptor, const struct iovec *iov, int vec_count, off_t offset) {
    off_t saved_offset;
    ssize_t result;
    int saved_errno;

    saved_offset = lseek(file_descriptor, 0, SEEK_CUR);
    if (saved_offset < 0)
        return -1;

    if (lseek(file_descriptor, offset, SEEK_SET) < 0)
        return -1;

    result = writev(file_descriptor, iov, vec_count);
    saved_errno = errno;

    lseek(file_descriptor, saved_offset, SEEK_SET);
    errno = saved_errno;

    return result;
}
