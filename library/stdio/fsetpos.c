/*
 * $Id: stdio_fsetpos.c,v 1.9 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fsetpos(FILE *stream, fpos_t *pos) {
    int result = EOF;
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

    if (fseek(stream, (long int) (*pos), SEEK_SET) == CHANGE_FILE_ERROR && __get_errno() != OK) {
        SHOWMSG("fseek failed");
        goto out;
    }

    result = OK;

out:

    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
