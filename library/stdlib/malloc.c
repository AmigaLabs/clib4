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

#include "../shared_library/clib4.h"

wmem_allocator_t *
__get_wmem_allocator(struct _clib4 *__clib4) {
    if (__clib4 == NULL)
        return NULL;

    /* Per-process allocator - created in stdlib_memory_init, destroyed in
     * libClose when this process closes the library. */
    return (wmem_allocator_t *) __clib4->__wmem_allocator;
}

void *
malloc(size_t size) {
    return __malloc_r(__CLIB4, size);
}

void *
__malloc_r(struct _clib4 *__clib4, size_t size) {
    return __malloc_aligned_r(__clib4, size, 16);
}

void *
__malloc_aligned_r(struct _clib4 *__clib4, size_t size, int32_t alignment) {
    void *result = NULL;
    wmem_allocator_t *allocator;

    if(size == 0) size = 4;

    // Prevent overflow
    if (size > SIZE_MAX) {
        __set_errno_r(__clib4, EOVERFLOW);
        goto out;
    }

    __memory_lock(__clib4);

    allocator = __get_wmem_allocator(__clib4);
    if (allocator == NULL) {
        __set_errno_r(__clib4, ENOMEM);
        __memory_unlock(__clib4);
        goto out;
    }

    result = wmem_alloc_aligned(allocator, size, alignment);

    if (!result)
        __set_errno_r(__clib4, ENOMEM);

    __memory_unlock(__clib4);

out:
    return (result);
}

void __memory_lock(struct _clib4 *__clib4) {
    /* The allocator is per-process, so the per-process mutex (shared by
     * all pthread threads, which run on the parent's _clib4 context) is
     * all the arbitration that is needed. The old global resource
     * semaphore is no longer taken here: it serialized every malloc/free
     * of every running clib4 program system-wide. */
    if(__clib4->memory_mutex)
        MutexObtain(__clib4->memory_mutex);
}

void __memory_unlock(struct _clib4 *__clib4) {
    if(__clib4->memory_mutex)
        MutexRelease(__clib4->memory_mutex);
}

void stdlib_memory_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->memory_mutex != NULL) {
        __delete_mutex(__clib4->memory_mutex);
        __clib4->memory_mutex = NULL;
    }

    /* Clear cached resource pointer */
    __clib4->__clib4_resource = NULL;

    LEAVE();
}

/* Called by clib4_init() in libOpen, before any other initialization function */
void stdlib_memory_init(void) {
    BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;
    struct Clib4Resource *res;

    ENTER();

    /* This function can run twice for statically linked programs (once
     * from clib4_init() in libOpen and once from the program's own ctor
     * list in call_main for binaries built with an older clib4), so it
     * must be idempotent. */
    if (__clib4->memory_mutex == NULL) {
        __clib4->memory_mutex = __create_mutex();
        if (__clib4->memory_mutex == NULL)
            goto out;
    }

    res = (APTR) OpenResource(RESOURCE_NAME);
    if (res == NULL) {
        __delete_mutex(__clib4->memory_mutex);
        __clib4->memory_mutex = NULL;
        goto out;
    }

    /* Cache the global resource pointer to avoid repeated OpenResource() calls */
    __clib4->__clib4_resource = res;

    /* Create the per-process allocator. All memory obtained through
     * malloc() and friends comes from here, and the whole allocator is
     * destroyed in libClose when this process closes the library, so
     * anything the program leaked is returned to the system at exit. */
    if (__clib4->__wmem_allocator == NULL) {
        __clib4->__wmem_allocator = wmem_allocator_new(__clib4->__wof_mem_allocator_type);
    }

    if (__clib4->__wmem_allocator == NULL) {
        __delete_mutex(__clib4->memory_mutex);
        __clib4->memory_mutex = NULL;
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
