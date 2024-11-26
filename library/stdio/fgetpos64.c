/*
 * $Id: stdio_fgetpos64.c,v 1.1 2024-07-20 17:36:32 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fgetpos64(FILE *stream, _fpos64_t *pos) {
    int result = EOF;
    _fpos64_t position;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);
    SHOWPOINTER(pos);

    assert(stream != NULL && pos != NULL);

    if (stream == NULL || pos == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    position = ftello64(stream);
    if (position == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("ftello64() didn't work.");

        goto out;
    }

    (*pos) = (_fpos64_t) position;

    result = OK;

out:

    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
