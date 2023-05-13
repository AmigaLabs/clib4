/*
 * $Id: stdlib_malloc.c,v 1.22 2022-04-03 14:09:00 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#undef malloc

void *
malloc(size_t size) {
    void *result = NULL;
    struct _clib2 *__clib2 = __CLIB2;

    __memory_lock(__clib2);

    result = wof_alloc(__clib2->__wof_allocator, size);

    __memory_unlock(__clib2);

    return (result);
}

void __memory_lock(struct _clib2 *__clib2) {
    if (__clib2->memory_semaphore != NULL)
        ObtainSemaphore(__clib2->memory_semaphore);
}

void __memory_unlock(struct _clib2 *__clib2) {
    if (__clib2->memory_semaphore != NULL)
        ReleaseSemaphore(__clib2->memory_semaphore);
}

STDLIB_DESTRUCTOR(stdlib_memory_exit) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    __memory_lock(__clib2);

    if (__clib2->__wof_allocator != NULL) {
        wof_allocator_destroy(__clib2->__wof_allocator);
        __clib2->__wof_allocator = NULL;
    }

    __memory_unlock(__clib2);

    if (__clib2->memory_semaphore != NULL) {
        __delete_semaphore(__clib2->memory_semaphore);
        __clib2->memory_semaphore = NULL;
    }

    LEAVE();
}

/* Second constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_memory_init) {
    BOOL success = FALSE;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    __clib2->memory_semaphore = __create_semaphore();
    if (__clib2->memory_semaphore == NULL)
        goto out;

    __clib2->__wof_allocator = wof_allocator_new();
    if (__clib2->__wof_allocator == NULL) {
        goto out;
    }

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
