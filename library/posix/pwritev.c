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
#include <sys/stat.h>
#include <unistd.h>

/* Source of the zero bytes that fill a gap before a write past the end. */
static const char zero_block[512];

/* Writes zeros to fd until its position reaches the given length. Seeking past
 * the end of a file fails unless unix path semantics are on, so the gap is
 * written instead. */
static int
fill_gap(int file_descriptor, off_t from, off_t to) {
    off_t remaining = to - from;

    while (remaining > 0) {
        size_t chunk = remaining > (off_t) sizeof(zero_block) ? sizeof(zero_block) : (size_t) remaining;
        ssize_t written = write(file_descriptor, zero_block, chunk);

        if (written <= 0) {
            if (written == 0)
                errno = EIO;
            return -1;
        }
        remaining -= written;
    }

    return 0;
}

ssize_t
pwritev(int file_descriptor, const struct iovec *iov, int vec_count, off_t offset) {
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

    saved_offset = lseek(file_descriptor, 0, SEEK_CUR);
    if (saved_offset < 0)
        return -1;

    /* Writing past the end of a regular file leaves a gap of zeros, as
     * pwrite() does. */
    if (fstat(file_descriptor, &st) == 0 && S_ISREG(st.st_mode) && offset > st.st_size) {
        if (lseek(file_descriptor, 0, SEEK_END) < 0 || fill_gap(file_descriptor, st.st_size, offset) < 0) {
            saved_errno = errno;
            lseek(file_descriptor, saved_offset, SEEK_SET);
            errno = saved_errno;
            return -1;
        }
    }

    if (lseek(file_descriptor, offset, SEEK_SET) < 0) {
        saved_errno = errno;
        lseek(file_descriptor, saved_offset, SEEK_SET);
        errno = saved_errno;
        return -1;
    }

    result = writev(file_descriptor, iov, vec_count);
    saved_errno = errno;

    lseek(file_descriptor, saved_offset, SEEK_SET);
    errno = saved_errno;

    return result;
}
