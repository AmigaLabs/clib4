/*
 * $Id: stdio_flockfile.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
flockfile(FILE *stream) {
    struct iob *file = (struct iob *) stream;

    ENTER();
    SHOWPOINTER(stream);

    assert(stream != NULL);

    __check_abort();

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
        ObtainSemaphore(file->iob_Lock);
        SET_FLAG(file->iob_Flags, IOBF_LOCKED);
    }

out:
    LEAVE();
    return;
}
