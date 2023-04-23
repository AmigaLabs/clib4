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
    __getclib2()->x[-1] = (__getclib2()->n << 16) | (__getclib2()->i << 8) | __getclib2()->j;
    return __getclib2()->x - 1;
}

void
loadstate(uint32_t *state) {
    __getclib2()->x = state + 1;
    __getclib2()->n = __getclib2()->x[-1] >> 16;
    __getclib2()->i = (__getclib2()->x[-1] >> 8) & 0xff;
    __getclib2()->j = __getclib2()->x[-1] & 0xff;
}

long
random(void) {
    long k;

    ENTER();

    ObtainSemaphore(__getclib2()->__random_lock);

    if (__getclib2()->n == 0) {
        k = __getclib2()->x[0] = lcg31(__getclib2()->x[0]);
        goto end;
    }
    __getclib2()->x[__getclib2()->i] += __getclib2()->x[__getclib2()->j];
    k = __getclib2()->x[__getclib2()->i]>>1;
    if (++__getclib2()->i == __getclib2()->n)
        __getclib2()->i = 0;
    if (++__getclib2()->j == __getclib2()->n)
        __getclib2()->j = 0;

end:
    ReleaseSemaphore(__getclib2()->__random_lock);

    RETURN(k);
    return k;
}