/*
 * $Id: stdio_unlockfile.c,v 1.3 2022-08-20 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__unlockfile(FILE *stream, int c) {
    funlockfile(stream);
    return (c);
}
