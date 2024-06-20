/*
 * $Id: stdio_funlockfile.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
funlockfile(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        __set_errno(EBADF);
        goto out;
    }

    if (file->iob_Lock != NULL && FLAG_IS_SET(file->iob_Flags, IOBF_LOCKED)) {
        ReleaseSemaphore(file->iob_Lock);
        CLEAR_FLAG(file->iob_Flags, IOBF_LOCKED);
    }
#if 0
    else {
        if (file->iob_Lock != NULL)
            SHOWMSG("file->iob_Lock is NULL!");
        if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_LOCKED))
            SHOWMSG("File is not locked or already unlocked!");
    }
#endif
out:
    return;
}
