/*
 * $Id: stdlib_malloc.c,v 1.23 2024-07-12 14:09:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include "mimalloc.h"

extern void _mi_auto_process_init(void);
extern void mi_cdecl _mi_auto_process_done(void) mi_attr_noexcept;

void *
malloc(size_t size) {
    return mi_malloc(size);
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

#if 0
    Printf("_mi_auto_process_exit\n");
    _mi_auto_process_done();
    Printf("_mi_auto_process_exit ok\n");
#endif

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

#if 0
    Printf("_mi_auto_process_init\n");
    _mi_auto_process_init();
    Printf("_mi_auto_process_init ok\n");
#endif

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
