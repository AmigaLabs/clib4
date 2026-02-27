/*
 * $Id: __std.c,v 1.1 2025-05-07 12:04:22 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
__stdin() {
    return (FILE *) __CLIB4->__iob[0];
}

FILE *
__stdout() {
    return (FILE *) __CLIB4->__iob[1];
}

FILE *
__stderr() {
    return (FILE *) __CLIB4->__iob[2];
}