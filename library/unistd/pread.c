/*
 * $Id: unistd_pread.c,v 1.0 2021-02-21 23:05:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

ssize_t
pread(int fd, void *buf, size_t n, off_t off) {
    off_t cur_pos;
    ssize_t num_read;

    if ((cur_pos = lseek(fd, 0, SEEK_CUR)) == (off_t) -1)
        return -1;

    if (lseek(fd, off, SEEK_SET) == (off_t) -1) {
        /*
         * AmigaOS ChangeFilePosition fails when seeking past EOF.
         * POSIX: pread at or past EOF must return 0, not an error.
         * Check if offset is at or past end of file.
         */
        off_t end_pos = lseek(fd, 0, SEEK_END);
        if (end_pos != (off_t) -1 && off >= end_pos) {
            lseek(fd, cur_pos, SEEK_SET);
            return 0;
        }
        /* Real seek error — restore position and propagate */
        lseek(fd, cur_pos, SEEK_SET);
        return -1;
    }

    num_read = read(fd, buf, n);

    if (lseek(fd, cur_pos, SEEK_SET) == (off_t) -1)
        return -1;

    return num_read;
}
