/*
 * $Id: stdio_clearerr.c,v 1.6 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef clearerr

void
clearerr(FILE *stream) {
    struct _clib4 *__clib4 = __CLIB4;
    __clearerr_r(__clib4, stream);
}

void
__clearerr_r(struct _clib4 *__clib4, FILE *stream) {
    struct iob *file = (struct iob *) stream;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    CLEAR_FLAG(file->iob_Flags, IOBF_ERROR);
    CLEAR_FLAG(file->iob_Flags, IOBF_EOF_REACHED);
    CLEAR_FLAG(file->iob_Flags2, __SERR);

out:

    LEAVE();
}
