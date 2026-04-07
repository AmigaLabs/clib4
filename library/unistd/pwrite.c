/*
 * $Id: unistd_pwrite.c,v 1.1 2022-03-04 23:09:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

ssize_t
pwrite(int fd, const void *buf, size_t n, off_t off) {
    ENTER();
    SHOWVALUE(fd);
    SHOWVALUE(n);
    SHOWVALUE(off);
    ssize_t result = ERROR;
    off_t cur_pos;

    if ((cur_pos = lseek(fd, 0, SEEK_CUR)) == (off_t)-1)
        goto out;

    if (lseek(fd, off, SEEK_SET) == (off_t)-1) {
        /*
         * AmigaOS ChangeFilePosition fails when seeking past EOF.
         * POSIX: pwrite past EOF extends the file with zero-fill.
         * Use ftruncate to extend, then retry the seek.
         */
        off_t end_pos = lseek(fd, 0, SEEK_END);
        if (end_pos != (off_t)-1 && off >= end_pos) {
            if (ftruncate(fd, off) == 0 &&
                lseek(fd, off, SEEK_SET) != (off_t)-1) {
                result = write(fd, buf, n);
                lseek(fd, cur_pos, SEEK_SET);
                goto out;
            }
        }
        /* Real error — restore position */
        lseek(fd, cur_pos, SEEK_SET);
        goto out;
    }

    result = write(fd, buf, n);

    if (lseek(fd, cur_pos, SEEK_SET) == (off_t)-1)
        result = ERROR;

out:

    RETURN(result);
    return result;
}
