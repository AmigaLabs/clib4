/*
 * $Id: unistd_pwrite64.c,v 1.0 2023-03-03 23:09:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/* off_t and off64_t are both int64_t, so delegate to pwrite */
ssize_t
pwrite64(int fd, const void *buf, size_t n, off64_t off) {
    return pwrite(fd, buf, n, (off_t) off);
}
