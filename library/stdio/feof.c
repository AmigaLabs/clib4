/*
 * $Id: stdio_feof.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef feof

int
feof(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);

        result = 0;

        goto out;
    }

    assert(__is_valid_iob(__clib2, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    flockfile(stream);

    result = FLAG_IS_SET(file->iob_Flags, IOBF_EOF_REACHED);

    funlockfile(stream);

out:

    RETURN(result);
    return (result);
}
