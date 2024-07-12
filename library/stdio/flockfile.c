/*
 * $Id: stdio_flockfile.c,v 1.5 2024-06-19 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
flockfile(FILE *stream) {
    return __flockfile_r(__CLIB4, stream);
}

void
__flockfile_r(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;

    __check_abort_f(__clib4);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);
        goto out;
    }

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        __set_errno(EBADF);
        goto out;
    }

    if (file->iob_Lock != NULL) {
        MutexAttempt(file->iob_Lock);
        SET_FLAG(file->iob_Flags, IOBF_LOCKED);
        file->iob_TaskLock = FindTask(NULL);
    }

out:
    return;
}