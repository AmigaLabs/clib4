/*
 * $Id: stdio_ftrylockfile.c,v 1.6 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
ftrylockfile(FILE *stream) {
    struct _clib4 *__clib4 = __CLIB4;

    return __ftrylockfile_r(__clib4, stream);
}

int
__ftrylockfile_r(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = ERROR;

    ENTER();
    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (FLAG_IS_SET(file->iob_Flags, IOBF_LOCKED)) {
        SHOWMSG("this file is already locked");

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    if (file->iob_Lock != NULL && CANNOT AttemptSemaphore(file->iob_Lock))
        goto out;

    SET_FLAG(file->iob_Flags, IOBF_LOCKED);
    file->iob_TaskLock = (struct Task *) __clib4->self;

    result = OK;

out:

    RETURN(result);
    return (result);
}
