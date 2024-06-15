/*
 * $Id: stdio_fsetpos64.c,v 1.0 2021-02-05 18:05:55 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fsetpos64(FILE *stream, _fpos64_t *pos) {
    int result = EOF;

    ENTER();

    SHOWPOINTER(stream);
    SHOWPOINTER(pos);

    assert(stream != NULL && pos != NULL);

    if (stream == NULL || pos == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        RETURN(result);
        return (result);
    }

    flockfile(stream);

    if (fseeko64(stream, (long int) (*pos), SEEK_SET) == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("fseek failed");
        goto out;
    }

    result = OK;

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
