/*
 * $Id: stdlib_free.c,v 1.15 2022-04-03 12:04:25 clib4devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

void
__free_r(struct _clib4 *__clib4, void *ptr) {
	if (ptr == NULL || __clib4 == NULL)
		return;

	if (__clib4->__wmem_allocator == NULL) {
		return;
	}

	__memory_lock(__clib4);
    wmem_free(__clib4->__wmem_allocator, ptr);

	__memory_unlock(__clib4);
}

void
free(void *ptr) {
    struct _clib4 *__clib4 = __CLIB4;

    return __free_r(__clib4, ptr);
}
