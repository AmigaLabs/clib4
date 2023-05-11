/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__stdio_lock(void) {
    struct _clib2 *__clib2 = __CLIB2;

    assert(__clib2->stdio_lock != NULL);

    if (__clib2->stdio_lock != NULL)
        ObtainSemaphore(__clib2->stdio_lock);
}

void
__stdio_unlock(void) {
    struct _clib2 *__clib2 = __CLIB2;

    assert(__clib2->stdio_lock != NULL);

    if (__clib2->stdio_lock != NULL)
        ReleaseSemaphore(__clib2->stdio_lock);
}

void
__stdio_lock_exit(void) {
    struct _clib2 *__clib2 = __CLIB2;
    __delete_semaphore(__clib2->stdio_lock);
    __clib2->stdio_lock = NULL;
}

int
__stdio_lock_init(void) {
    int result = ERROR;

    __CLIB2->stdio_lock = __create_semaphore();
    if (__CLIB2->stdio_lock == NULL)
        goto out;

    result = OK;
out:

    return result;
}
