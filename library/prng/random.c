/*
 * $Id: stdlib_random.c,v 1.0 2021-03-22 20:10:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

uint32_t
lcg31(uint32_t x) {
    return (1103515245 * x + 12345) & 0x7fffffff;
}

uint64_t
lcg64(uint64_t x) {
    return 6364136223846793005ull * x + 1;
}

void *
savestate() {
    __CLIB2->x[-1] = (__CLIB2->n << 16) | (__CLIB2->i << 8) | __CLIB2->j;
    return __CLIB2->x - 1;
}

void
loadstate(uint32_t *state) {
    __CLIB2->x = state + 1;
    __CLIB2->n = __CLIB2->x[-1] >> 16;
    __CLIB2->i = (__CLIB2->x[-1] >> 8) & 0xff;
    __CLIB2->j = __CLIB2->x[-1] & 0xff;
}

long
random(void) {
    long k;

    ENTER();

    ObtainSemaphore(__CLIB2->__random_lock);

    if (__CLIB2->n == 0) {
        k = __CLIB2->x[0] = lcg31(__CLIB2->x[0]);
        goto end;
    }
    __CLIB2->x[__CLIB2->i] += __CLIB2->x[__CLIB2->j];
    k = __CLIB2->x[__CLIB2->i]>>1;
    if (++__CLIB2->i == __CLIB2->n)
        __CLIB2->i = 0;
    if (++__CLIB2->j == __CLIB2->n)
        __CLIB2->j = 0;

end:
    ReleaseSemaphore(__CLIB2->__random_lock);

    RETURN(k);
    return k;
}