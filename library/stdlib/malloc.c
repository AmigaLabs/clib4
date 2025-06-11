/*
 * $Id: stdlib_malloc.c,v 1.23 2024-07-12 14:09:00 clib4devs Exp $
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

void *
malloc(size_t size) {
    return __malloc_r(__CLIB4, size);
}

void *
__malloc_r(struct _clib4 *__clib4, size_t size) {
    ENTER();
    void *result = NULL;

    __memory_lock(__clib4);

    result = wmem_alloc(__clib4->__wmem_allocator, size);

    if (!result)
        __set_errno_r(__clib4, ENOMEM);

    __memory_unlock(__clib4);

    LEAVE();
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

STDLIB_DESTRUCTOR(stdlib_memory_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __memory_lock(__clib4);

    if (__clib4->__wmem_allocator != NULL) {
        SHOWMSG("Destroying Memory Allocator");
        wmem_destroy_allocator(__clib4->__wmem_allocator);
#if MEMORY_DEBUG
        if (__clib4->allocated_memory_by_malloc > 0) {
            Printf("WARNING: There are %ld unfreed malloc!\n", __clib4->allocated_memory_by_malloc);
        }
#endif
        SHOWMSG("Done");
        __clib4->__wmem_allocator = NULL;
    }

    __memory_unlock(__clib4);

    if (__clib4->memory_semaphore != NULL) {
        __delete_semaphore(__clib4->memory_semaphore);
        __clib4->memory_semaphore = NULL;
    }

    LEAVE();
}

/* Second constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_memory_init) {
    BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __clib4->memory_semaphore = __create_semaphore();
    if (__clib4->memory_semaphore == NULL)
        goto out;

    __clib4->__wmem_allocator = wmem_allocator_new(__clib4->__wof_mem_allocator_type); // make this dynamic
    if (__clib4->__wmem_allocator == NULL) {
        __delete_semaphore(__clib4->memory_semaphore);
        __clib4->memory_semaphore = NULL;
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
