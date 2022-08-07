/*
 * $Id: stdlib_jrand48.c,v 1.0 2021-02-23 10:36:26 clib2devs Exp $
*/
#define _XOPEN_SOURCE

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

long
jrand48(unsigned short s[3]) {
    return (int32_t)(__rand48_step(s, __seed48 + 3) >> 16);
}

