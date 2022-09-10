/*
 * $Id: stdlib_aligned_alloc.c,v 1.0 2021-02-22 19:26:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <malloc.h>
#include <stdint.h>

void *
aligned_alloc(size_t align, size_t size) {
    ENTER();

    /* align must be a power of 2 */
    /* size must be a multiple of align */
    if ((align & -align) != align) {
        __set_errno(EINVAL);
        return NULL;
    }

    if (size > SIZE_MAX - align) {
        __set_errno(ENOMEM);
        RETURN(NULL);
        return NULL;
    }

    if (align <= SIZE_ALIGN) {
        LEAVE();
        return malloc(size);
    }

    LEAVE();

    return memalign(align, size);
}
