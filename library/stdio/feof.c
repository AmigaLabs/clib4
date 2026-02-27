/*
 * $Id: stdio_feof.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef feof

int
feof(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno_r(__clib4, EFAULT);
        result = 0;
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));

    int locked = __ftrylockfile_r(__clib4, stream);

    result = FLAG_IS_SET(file->iob_Flags, IOBF_EOF_REACHED);

    if (locked == OK)
        __funlockfile_r(__clib4, stream);

out:

    RETURN(result);
    return (result);
}
