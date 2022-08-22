/*
 * $Id: unistd_readlink.c,v 1.9 2006-11-13 09:25:28 clib2devs Exp $
*/

#define _DEFAULT_SOURCE

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

void *
reallocarray(void *ptr, size_t m, size_t n) {
    if (n && m > -1 / n) {
        __set_errno(ENOMEM);
        return 0;
    }

    return realloc(ptr, m * n);
}
