/*
 * $Id: stdio_fgetpos64.c,v 1.0 2021-02-05 17:36:32 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fgetpos64(FILE *stream, _fpos64_t *pos) {
    int result = EOF;
    _fpos64_t position;

    ENTER();

    SHOWPOINTER(stream);
    SHOWPOINTER(pos);

    assert(stream != NULL && pos != NULL);

    flockfile(stream);

    if (stream == NULL || pos == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    position = ftello64(stream);
    if (position == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("ftello64() didn't work.");

        goto out;
    }

    (*pos) = (_fpos64_t) position;

    result = OK;

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
