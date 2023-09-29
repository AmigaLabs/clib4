/*
 * $Id: stdlib_srandom.c,v 1.0 2021-01-24 20:10:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__srandom(unsigned seed) {
    int k;
    uint64_t s = seed;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWVALUE(seed);

    if (__clib4->n == 0) {
        __clib4->x[0] = s;
        return;
    }
    __clib4->i = __clib4->n == 31 || __clib4->n == 7 ? 3 : 1;
    __clib4->j = 0;
    for (k = 0; k < __clib4->n; k++) {
        s = lcg64(s);
        __clib4->x[k] = s>>32;
    }
    /* make sure x contains at least one odd number */
    __clib4->x[0] |= 1;

    LEAVE();
}

void
srandom(unsigned int seed)  {
    struct _clib4 *__clib4 = __CLIB4;

    ObtainSemaphore(__clib4->__random_lock);

    __srandom(seed);

    ReleaseSemaphore(__clib4->__random_lock);
}