/*
 * $Id: stdlib_malloc.c,v 1.22 2024-04-09 14:09:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#undef malloc

void *
malloc(size_t size) {
    void *result = NULL;
    struct _clib4 *__clib4 = __CLIB4;

    __memory_lock(__clib4);

    result = wof_alloc(__clib4->__wof_allocator, size);

    __memory_unlock(__clib4);

    return (result);
}

void __memory_lock(struct _clib4 *__clib4) {
    if (__clib4->memory_semaphore != NULL)
        ObtainSemaphore(__clib4->memory_semaphore);
}

void __memory_unlock(struct _clib4 *__clib4) {
    if (__clib4->memory_semaphore != NULL)
        ReleaseSemaphore(__clib4->memory_semaphore);
}

void
_stdlib_memory_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __memory_lock(__clib4);

    if (__clib4->__wof_allocator != NULL) {
        wof_allocator_destroy(__clib4->__wof_allocator);
        __clib4->__wof_allocator = NULL;
    }

    __memory_unlock(__clib4);

    if (__clib4->__wof_allocator_semaphore != NULL) {
        __delete_semaphore(__clib4->__wof_allocator_semaphore);
        __clib4->__wof_allocator_semaphore = NULL;
    }

    if (__clib4->memory_semaphore != NULL) {
        __delete_semaphore(__clib4->memory_semaphore);
        __clib4->memory_semaphore = NULL;
    }

    LEAVE();
}

/* Second constructor called by _init */
void
_stdlib_memory_init(void) {
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __clib4->memory_semaphore = __create_semaphore();
    if (__clib4->memory_semaphore == NULL) {
        SHOWMSG("Cannot create memory semapore. Exit");
        goto out;
    }

    __clib4->__wof_allocator_semaphore = __create_semaphore();
    if (__clib4->__wof_allocator_semaphore == NULL) {
        SHOWMSG("Cannot wof allocate semapore. Exit");
        goto out;
    }

    __clib4->__wof_allocator = wof_allocator_new();
    if (__clib4->__wof_allocator == NULL) {
        SHOWMSG("FAILED TO CREATE WOF ALLOCATOR! EXIT");
        goto out;
    }

out:

    LEAVE();
}
