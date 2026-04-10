/*
 * $Id: unistd_pread64.c,v 1.0 2023-03-03 23:05:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/* off_t and off64_t are both int64_t, so delegate to pread */
ssize_t
pread64(int fd, void *buf, size_t n, off64_t off) {
    return pread(fd, buf, n, (off_t) off);
}
