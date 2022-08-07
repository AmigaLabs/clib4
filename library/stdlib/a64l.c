/*
 * $Id: stdlib_a64l.c,v 1.0 2022-08-06 12:04:25 clib2devs Exp $
*/
#define _XOPEN_SOURCE

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */


static const char digits[] =
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

long
a64l(const char *s) {
    int e;
    uint32_t x = 0;
    for (e = 0; e < 36 && *s; e += 6, s++) {
        const char *d = strchr(digits, *s);
        if (!d) break;
        x |= (uint32_t)(d - digits) << e;
    }
    return (int32_t) x;
}