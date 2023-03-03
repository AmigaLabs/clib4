/*
 * $Id: unistd_pread.c,v 1.0 2021-02-21 23:05:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

ssize_t
pread(int fd, void *buf, size_t n, off_t off) {
    off_t cur_pos;
    ssize_t num_read;

    if ((cur_pos = lseek(fd, 0, SEEK_CUR)) == (off_t) - 1)
        return -1;

    if (lseek(fd, off, SEEK_SET) == (off_t) - 1)
        return -1;

    num_read = read(fd, buf, n);

    if (lseek(fd, cur_pos, SEEK_SET) == (off_t) - 1)
        return -1;

    return (ssize_t) num_read;
}
