/*
 * $Id: stdlib_srandom.c,v 1.0 2021-01-24 20:10:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__srandom(unsigned seed) {
    int k;
    uint64_t s = seed;

    ENTER();
    SHOWVALUE(seed);

    if (__getclib2()->n == 0) {
        __getclib2()->x[0] = s;
        return;
    }
    __getclib2()->i = __getclib2()->n == 31 || __getclib2()->n == 7 ? 3 : 1;
    __getclib2()->j = 0;
    for (k = 0; k < __getclib2()->n; k++) {
        s = lcg64(s);
        __getclib2()->x[k] = s>>32;
    }
    /* make sure x contains at least one odd number */
    __getclib2()->x[0] |= 1;

    LEAVE();
}

void
srandom(unsigned int seed)
{
    ObtainSemaphore(__getclib2()->__random_lock);

    __srandom(seed);

    ReleaseSemaphore(__getclib2()->__random_lock);
}