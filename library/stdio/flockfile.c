/*
 * $Id: stdio_flockfile.c,v 1.6 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
flockfile(FILE *stream) {
    __flockfile_r(__CLIB4, stream);
}

void
__flockfile_r(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;

    __set_errno_r(__clib4, 0);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (file->iob_Lock != NULL) {
        ObtainSemaphore(file->iob_Lock);
        SET_FLAG(file->iob_Flags, IOBF_LOCKED);
        file->iob_TaskLock = (struct Task *) __clib4->self;
    }
out:
    return;
}