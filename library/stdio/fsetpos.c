/*
 * $Id: stdio_fsetpos.c,v 1.8 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fsetpos(FILE *stream, fpos_t *pos) {
    int result = EOF;

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

    if (fseek(stream, (long int) (*pos), SEEK_SET) == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("fseek failed");
        goto out;
    }

    result = OK;

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
