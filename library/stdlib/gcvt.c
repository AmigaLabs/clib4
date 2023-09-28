/*
 * $Id: stdlib_gcvt.c,v 1.0 2022-08-22 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
gcvt(double x, int n, char *b) {
    sprintf(b, "%.*g", n, x);
    return b;
}
