/*
 * $Id: stdlib_calloc.c,v 1.8 2024-07-02 12:59:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

void *
calloc(size_t num_elements, size_t element_size) {
    void *result = NULL;
    size_t total_size = 0;

    if (num_elements != 0) {
        total_size = num_elements * element_size;
        if (((num_elements | element_size) & ~(size_t) 0xffff) && (total_size / num_elements != element_size)) {
            __set_errno(ENOMEM);
            return NULL;
        }
    }

    result = malloc(total_size);
    if (result != NULL)
        memset(result, 0, total_size);
    else {
        D(("Memory allocation of %ld bytes failed:", total_size));
    }

    return result;
}
