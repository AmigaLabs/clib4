/*
 * $Id: stdlib_l64a.c,v 1.0 2022-08-06 12:04:25 clib2devs Exp $
*/
#define _XOPEN_SOURCE

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

static const char digits[] =
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char *
l64a(long x0) {
    static char s[7];
    char *p;
    uint32_t x = x0;
    for (p = s; x; p++, x >>= 6)
        *p = digits[x & 63];
    *p = 0;
    return s;
}