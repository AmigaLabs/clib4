/*
 * $Id: stdio_rewind.c,v 1.4 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
rewind(FILE *stream) {
    ENTER();
    assert(stream != NULL);

    flockfile(stream);

    clearerr(stream);

    fseek(stream, 0, SEEK_SET);

    funlockfile(stream);
    LEAVE();
}
