/*
 * $Id: stdlib_seed48.c,v 1.0 2022-08-06 10:36:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

unsigned short *
seed48(unsigned short s[3]) {
    static unsigned short p[3];
    memcpy(p, __seed48, sizeof p);
    memcpy(__seed48, s, sizeof p);
    return p;
}