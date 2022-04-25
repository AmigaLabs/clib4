/*
 * $Id: stdio_fgetpos.c,v 1.8 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fgetpos(FILE *stream, fpos_t *pos) {
    int result = EOF;
    fpos_t position;

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

    position = ftell(stream);
    if (position == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("ftell() didn't work.");

        goto out;
    }

    (*pos) = (fpos_t) position;

    result = OK;

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
