/*
 * $Id: mman_mmap.c,v 1.0 2021-01-18 20:17:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>

int msync(void *addr, size_t len, int flags) {
    /* Nothing for now.. */
    (void)(addr);
    (void)(len);
    (void)(flags);

    return 0;
}