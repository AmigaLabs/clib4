/*
 * $Id: stdio_unlockfile.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__unlockfile(FILE *stream, int c) {
    ENTER();

    funlockfile(stream);

    RETURN(c);
    return (c);
}
