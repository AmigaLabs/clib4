/*
 * $Id: stdlib_erand48.c,v 1.0 2022-08-06 10:36:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

double
erand48(unsigned short s[3]) {
    union {
        uint64_t u;
        double f;
    } x = {0x3ff0000000000000ULL | __rand48_step(s, __seed48 + 3) << 4};
    return x.f - 1.0;
}