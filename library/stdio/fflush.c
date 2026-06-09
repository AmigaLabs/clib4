/*
 * $Id: stdio_fflush.c,v 1.12 2025-01-01 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fflush(FILE *stream) {
    struct _clib4 *__clib4 = __CLIB4;
    return __fflush_r(__clib4, stream);
}

int
__fflush_r(struct _clib4 *__clib4, FILE *stream) {
    int result = EOF;

    ENTER();

    SHOWPOINTER(stream);

    /* Flush a particular stream? */
    if (stream != NULL) {
        struct iob *file = (struct iob *) stream;

        __flockfile_r(__clib4, stream);

        if (__sflush(__clib4, file) != 0) {
            __funlockfile_r(__clib4, stream);
            RETURN(result);
            return result;
        }

        __funlockfile_r(__clib4, stream);

    } else {
        /* Flush all open streams using _fwalk over the glue list */
        __stdio_lock(__clib4);

        if (_fwalk(__clib4, __sflush) != 0) {
            __stdio_unlock(__clib4);
            goto out;
        }

        __stdio_unlock(__clib4);
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
