/*
 * $Id: malloc_memalign.c,v 1.1 2025-06-07 10:52:18 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <stdint.h>
#include <malloc.h>

void *
memalign(size_t alignment, size_t size) {

    // Alignment must be powers of two
    if (alignment == 0 || (alignment & -alignment) != alignment) {
        __set_errno(EINVAL);
        return NULL;
    }

    return __malloc_aligned_r(__CLIB4, size, alignment);
}
