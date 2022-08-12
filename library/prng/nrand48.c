/*
 * $Id: stdlib_nrand48.c,v 1.0 2022-08-06 10:36:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

long
nrand48(unsigned short s[3]) {
    return __rand48_step(s, __seed48 + 3) >> 17;
}