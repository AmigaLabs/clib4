/*
 * $Id: stdlib_random.c,v 1.0 2021-03-22 20:10:26 clib2devs Exp $
*/
#define _XOPEN_SOURCE

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
    __global_clib2->x[-1] = (__global_clib2->n << 16) | (__global_clib2->i << 8) | __global_clib2->j;
    return __global_clib2->x - 1;
}

void
loadstate(uint32_t *state) {
    __global_clib2->x = state + 1;
    __global_clib2->n = __global_clib2->x[-1] >> 16;
    __global_clib2->i = (__global_clib2->x[-1] >> 8) & 0xff;
    __global_clib2->j = __global_clib2->x[-1] & 0xff;
}

long
random(void) {
    long k;

    ENTER();

    ObtainSemaphore(__global_clib2->__random_lock);

    if (__global_clib2->n == 0) {
        k = __global_clib2->x[0] = lcg31(__global_clib2->x[0]);
        goto end;
    }
    __global_clib2->x[__global_clib2->i] += __global_clib2->x[__global_clib2->j];
    k = __global_clib2->x[__global_clib2->i]>>1;
    if (++__global_clib2->i == __global_clib2->n)
        __global_clib2->i = 0;
    if (++__global_clib2->j == __global_clib2->n)
        __global_clib2->j = 0;

end:
    ReleaseSemaphore(__global_clib2->__random_lock);

    RETURN(k);
    return k;
}