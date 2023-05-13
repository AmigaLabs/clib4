/*
 * $Id: stdio_findvacantfdentry.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef NDEBUG

BOOL
__is_valid_fd(struct _clib2 *__clib2, struct fd *fd) {
    BOOL result = FALSE;

    __stdio_lock(__clib2);

    if (__clib2->__fd != NULL && __clib2->__num_fd > 0) {
        int i;

        for (i = 0; i < __clib2->__num_fd; i++) {
            if (__clib2->__fd[i] == fd) {
                result = TRUE;
                break;
            }
        }
    }

    __stdio_unlock(__clib2);

    return (result);
}

#endif /* NDEBUG */

int
__find_vacant_fd_entry(struct _clib2 *__clib2) {
    int result = ERROR;
    int i;

    assert(__clib2->__fd != NULL || __clib2->__num_fd == 0);

    for (i = 0; i < __clib2->__num_fd; i++) {
        if (FLAG_IS_CLEAR(__clib2->__fd[i]->fd_Flags, FDF_IN_USE)) {
            result = i;
            break;
        }
    }

    return (result);
}
