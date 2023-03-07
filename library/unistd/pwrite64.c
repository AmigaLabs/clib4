/*
 * $Id: unistd_pwrite64.c,v 1.0 2023-03-03 23:09:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

ssize_t
pwrite64(int fd, const void *buf, size_t n, off64_t off) {
    off64_t cur_pos;
    ssize_t num_written;

    if ((cur_pos = lseek64(fd, 0, SEEK_CUR)) == (off64_t) - 1)
        return -1;

    if (lseek64(fd, off, SEEK_SET) == (off64_t) - 1)
        return -1;

    num_written = write(fd, buf, n);

    if (lseek64(fd, cur_pos, SEEK_SET) == (off64_t) - 1)
        return -1;

    return (ssize_t) num_written;
}
