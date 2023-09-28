/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__stdio_lock(struct _clib4 *__clib4) {
    assert(__clib4->stdio_lock != NULL);

    if (__clib4->stdio_lock != NULL)
        ObtainSemaphore(__clib4->stdio_lock);
}

void
__stdio_unlock(struct _clib4 *__clib4) {
    assert(__clib4->stdio_lock != NULL);

    if (__clib4->stdio_lock != NULL)
        ReleaseSemaphore(__clib4->stdio_lock);
}

void
__stdio_lock_exit(struct _clib4 *__clib4) {
    __delete_semaphore(__clib4->stdio_lock);
    __clib4->stdio_lock = NULL;
}

int
__stdio_lock_init(struct _clib4 *__clib4) {
    int result = ERROR;

    __clib4->stdio_lock = __create_semaphore();
    if (__clib4->stdio_lock == NULL)
        goto out;

    result = OK;
out:

    return result;
}
