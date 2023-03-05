/*
 * $Id: unistd_pwrite.c,v 1.1 2022-03-04 23:09:27 clib2devs Exp $
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

    __set_errno(0);

    if ((cur_pos = lseek(fd, 0, SEEK_CUR)) == (off_t)-1)
        goto out;

    if (lseek(fd, off, SEEK_SET) == (off_t)-1)
        goto out;

    result = write(fd, buf, n);

    if (lseek(fd, cur_pos, SEEK_SET) == (off_t)-1)
        result = ERROR;

out:

    RETURN(result);
    return result;
}
