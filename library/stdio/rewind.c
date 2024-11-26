/*
 * $Id: stdio_rewind.c,v 1.5 2024-07-20 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
rewind(FILE *stream) {
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    assert(stream != NULL);

    __flockfile_r(__clib4, stream);

    __clearerr_r(__clib4, stream);

    fseek(stream, 0, SEEK_SET);

    __funlockfile_r(__clib4, stream);
    LEAVE();
}
