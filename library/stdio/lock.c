/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__stdio_lock(struct _clib4 *__clib4) {
    if (__clib4->stdio_lock != NULL)
        MutexObtain(__clib4->stdio_lock);
}

void
__stdio_unlock(struct _clib4 *__clib4) {
    if (__clib4->stdio_lock != NULL)
        MutexRelease(__clib4->stdio_lock);
}

void
__stdio_lock_exit(struct _clib4 *__clib4) {
    __delete_mutex(__clib4->stdio_lock);
    __clib4->stdio_lock = NULL;
}

int
__stdio_lock_init(struct _clib4 *__clib4) {
    int result = ERROR;

    __clib4->stdio_lock = __create_mutex();
    if (__clib4->stdio_lock == NULL)
        goto out;

    result = OK;
out:

    return result;
}
