/*
 * $Id: __std.c,v 1.2 2025-01-01 12:04:22 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
__stdin() {
    struct _clib4 *__clib4 = __CLIB4;
    return (FILE *) __clib4->__iob[0];
}

FILE *
__stdout() {
    struct _clib4 *__clib4 = __CLIB4;
    return (FILE *) __clib4->__iob[1];
}

FILE *
__stderr() {
    struct _clib4 *__clib4 = __CLIB4;
    return (FILE *) __clib4->__iob[2];
}