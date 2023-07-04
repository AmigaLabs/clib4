/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__stdio_lock(struct _clib2 *__clib2) {
    assert(__clib2->stdio_lock != NULL);

    if (__clib2->stdio_lock != NULL)
        ObtainSemaphore(__clib2->stdio_lock);
}

void
__stdio_unlock(struct _clib2 *__clib2) {
    assert(__clib2->stdio_lock != NULL);

    if (__clib2->stdio_lock != NULL)
        ReleaseSemaphore(__clib2->stdio_lock);
}

void
__stdio_lock_exit(struct _clib2 *__clib2) {
    __delete_semaphore(__clib2->stdio_lock);
    __clib2->stdio_lock = NULL;
}

int
__stdio_lock_init(struct _clib2 *__clib2) {
    int result = ERROR;

    __clib2->stdio_lock = __create_semaphore();
    if (__clib2->stdio_lock == NULL)
        goto out;

    result = OK;
out:

    return result;
}
