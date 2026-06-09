/*
 * $Id: stdio_fgetpos.c,v 1.9 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fgetpos(FILE *stream, fpos_t *pos) {
    int result = EOF;
    fpos_t position;
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

    position = ftell(stream);
    if (position == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("ftell() didn't work.");

        goto out;
    }

    (*pos) = position;

    result = OK;

out:

    RETURN(result);
    return (result);
}
