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

    __memory_lock();

    result = wof_alloc(__CLIB2->__wof_allocator, size);

    __memory_unlock();

    return (result);
}

void __memory_lock(void) {
    if (__CLIB2->memory_semaphore != NULL)
        ObtainSemaphore(__CLIB2->memory_semaphore);
}

void __memory_unlock(void) {
    if (__CLIB2->memory_semaphore != NULL)
        ReleaseSemaphore(__CLIB2->memory_semaphore);
}

STDLIB_DESTRUCTOR(stdlib_memory_exit) {
    ENTER();

    __memory_lock();

    if (__CLIB2->__wof_allocator != NULL) {
        wof_allocator_destroy(__CLIB2->__wof_allocator);
        __CLIB2->__wof_allocator = NULL;
    }

    __memory_unlock();

    if (__CLIB2->memory_semaphore != NULL) {
        __delete_semaphore(__CLIB2->memory_semaphore);
        __CLIB2->memory_semaphore = NULL;
    }

    LEAVE();
}

/* Second constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_memory_init) {
    BOOL success = FALSE;

    ENTER();

    __CLIB2->memory_semaphore = __create_semaphore();
    if (__CLIB2->memory_semaphore == NULL)
        goto out;

    __CLIB2->__wof_allocator = wof_allocator_new();
    if (__CLIB2->__wof_allocator == NULL) {
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
