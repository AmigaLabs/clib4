/*
 * $Id: fcntl_lseek.c,v 1.0 2022-03-30 12:04:22 clib4devs Exp $
*/
#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/* Our lseek is always 64 bit */
off64_t lseek64(int fd, off64_t offset, int whence) {
    return lseek(fd, offset, whence);
}