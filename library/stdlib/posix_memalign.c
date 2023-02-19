/*
 * $Id: stdlib_posix_memalign.c,v 1.0 2022-04-17 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
posix_memalign(void **res, size_t align, size_t len) {
    if (align < sizeof(void *)) {
        return EINVAL;
    }

    void *mem = aligned_alloc(align, len);
    if (!mem) {
        return errno;
    }
    *res = mem;

    return 0;
}
