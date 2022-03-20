/*
 * $Id: stdio_ferror.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef ferror

int
ferror(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);

        result = 0;

        goto out;
    }

    assert(__is_valid_iob(file));

    flockfile(stream);

    result = FLAG_IS_SET(file->iob_Flags, IOBF_ERROR);

    funlockfile(stream);

out:

    RETURN(result);
    return (result);
}
