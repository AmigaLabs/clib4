/*
 * $Id: stdlib_calloc.c,v 1.7 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#undef calloc

__static void *
__calloc(size_t num_elements, size_t element_size) {
    void *result = NULL;
    size_t total_size;

    /* This might overflow. */
    total_size = num_elements * element_size;

    /* No arithmetic overflow? */
    if (total_size >= num_elements) {
        result = malloc(total_size);
        if (result != NULL)
            memset(result, 0, total_size);
        else
            SHOWMSG("memory allocation failure");
    }
    /* Multiplying the number and size of elements overflows
     * the size_t range.
     */
    else {
        D(("calloc(num_elements=%ld, element_size=%ld) overflow"));
    }

    return (result);
}

void *
calloc(size_t num_elements, size_t element_size) {
    void *result;

    ENTER();
    SHOWVALUE(num_elements);
    SHOWVALUE(element_size);

    result = __calloc(num_elements, element_size);

    SHOWPOINTER(result);
    LEAVE();
    return (result);
}
