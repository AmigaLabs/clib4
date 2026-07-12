/*
 * $Id: stdlib_malloc_usable_size.c,v 1.0 2024-01-01 00:00:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

size_t
__malloc_usable_size_r(struct _clib4 *__clib4, const void *ptr) {
    size_t result = 0;
    wmem_allocator_t *allocator;

    if (ptr == NULL || __clib4 == NULL)
        return 0;

    __memory_lock(__clib4);

    allocator = __get_wmem_allocator(__clib4);
    if (allocator == NULL) {
        __memory_unlock(__clib4);
        return 0;
    }

    result = wmem_alloc_size(allocator, ptr);

    __memory_unlock(__clib4);

    return result;
}

size_t
malloc_usable_size(void *ptr) {
    return __malloc_usable_size_r(__CLIB4, ptr);
}
