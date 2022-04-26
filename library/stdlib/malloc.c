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

unsigned long NOCOMMON __maximum_memory_allocated;
unsigned long NOCOMMON __current_memory_allocated;
unsigned long NOCOMMON __maximum_num_memory_chunks_allocated;
unsigned long NOCOMMON __current_num_memory_chunks_allocated;

/* Wof Allocator */
wof_allocator_t NOCOMMON *__wof_allocator;

static struct SignalSemaphore *memory_semaphore;

void *
malloc(size_t size) {
    void *result = NULL;

    __memory_lock();

    result = wof_alloc(__wof_allocator, size);

    __memory_unlock();

    return (result);
}

void __memory_lock(void) {
    if (memory_semaphore != NULL)
        ObtainSemaphore(memory_semaphore);
}

void __memory_unlock(void) {
    if (memory_semaphore != NULL)
        ReleaseSemaphore(memory_semaphore);
}

STDLIB_DESTRUCTOR(stdlib_memory_exit) {
    ENTER();

    __memory_lock();

    if (__wof_allocator != NULL)
    wof_allocator_destroy(__wof_allocator);

    __memory_unlock();

    if (memory_semaphore != NULL) {
        __delete_semaphore(memory_semaphore);
        memory_semaphore = NULL;
    }

    LEAVE();
}

/* Second constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_memory_init) {
    BOOL success = FALSE;

    ENTER();

    memory_semaphore = __create_semaphore();
    if (memory_semaphore == NULL)
    goto out;

    __wof_allocator = wof_allocator_new();
    if (__wof_allocator == NULL) {
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
