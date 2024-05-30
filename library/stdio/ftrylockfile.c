/*
 * $Id: stdio_ftrylockfile.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
ftrylockfile(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    __check_abort_f(__clib4);

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

    if (file->iob_Lock != NULL && CANNOT AttemptSemaphore(file->iob_Lock))
    goto out;

    result = OK;

out:

    RETURN(result);
    return (result);
}
