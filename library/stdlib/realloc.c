/*
 * $Id: stdlib_realloc.c,v 1.11 2022-04-03 10:55:03 clib2devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

void *
realloc(void *ptr, size_t size) {
    void *result = NULL;
    BOOL locked = FALSE;
    struct _clib2 *__clib2 = __CLIB2;

    assert((int) size >= 0);

    if (ptr == NULL) {
        D(("calling malloc(%ld)", size));

        result = malloc(size);
    } else if (__clib2->__unix_path_semantics && size == 0) {
        D(("calling free(0x%08lx)", ptr));

        free(ptr);
    } else {
        result = wof_realloc(__clib2->__wof_allocator, ptr, size);
        if (result == NULL) {
            SHOWMSG("could not reallocate memory");
            goto out;
        }
    }

out:

    if (locked)
        __memory_unlock(__clib2);

    if (result == NULL)
        SHOWMSG("ouch! realloc failed");

    return (result);
}
