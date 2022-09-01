/*
 * $Id: stdlib_rand_r.c,v 1.2 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

static
unsigned temper(unsigned x) {
    x ^= x>>11;
    x ^= x<<7 & 0x9D2C5680;
    x ^= x<<15 & 0xEFC60000;
    x ^= x>>18;
    return x;
}

int
rand_r(unsigned int *seed) {
    ENTER();
    SHOWPOINTER(seed);

    __check_abort();

    int result = temper(*seed = *seed * 1103515245 + 12345)/2;

    RETURN(result);
    return result;
}
