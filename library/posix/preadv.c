/*
 * preadv() is built on readv() and lseek(), which clib4 provides. It moves the
 * file offset, reads, and restores the offset. Other users of the same
 * descriptor can observe the temporary offset.
 */

#ifndef _UIO_HEADERS_H
#include "uio_headers.h"
#endif /* _UIO_HEADERS_H */

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

ssize_t
preadv(int file_descriptor, const struct iovec *iov, int vec_count, off_t offset) {
    struct stat st;
    off_t saved_offset;
    ssize_t result;
    int saved_errno;

    /* Checked here, not left to lseek(): clib4's lseek clamps a negative
     * position to 0 instead of failing. */
    if (offset < 0) {
        errno = EINVAL;
        return -1;
    }

    /* Reading at or past the end of a regular file returns 0 without seeking.
     * Seeking past the end extends the file under unix path semantics, and
     * preadv() must not change the file. */
    if (fstat(file_descriptor, &st) == 0 && S_ISREG(st.st_mode) && offset >= st.st_size)
        return 0;

    saved_offset = lseek(file_descriptor, 0, SEEK_CUR);
    if (saved_offset < 0)
        return -1;

    if (lseek(file_descriptor, offset, SEEK_SET) < 0)
        return -1;

    result = readv(file_descriptor, iov, vec_count);
    saved_errno = errno;

    lseek(file_descriptor, saved_offset, SEEK_SET);
    errno = saved_errno;

    return result;
}
