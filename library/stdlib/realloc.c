/*
 * $Id: stdlib_realloc.c,v 1.11 2022-04-03 10:55:03 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    assert((int) size >= 0);

    __memory_lock(__clib4);

    if (ptr == NULL) {
        D(("calling malloc(%ld)", size));

        result = __malloc_r(__clib4, size);
    } else if (__clib4->__unix_path_semantics && size == 0) {
        D(("calling free(0x%08lx)", ptr));

        __free_r(__clib4, ptr);
    } else {
        result = wof_realloc(__clib4->__wof_allocator, ptr, size);
        if (result == NULL) {
            SHOWMSG("could not reallocate memory");
        }
    }

    __memory_unlock(__clib4);

    if (result == NULL)
        SHOWMSG("ouch! realloc failed");

    return result;
}
