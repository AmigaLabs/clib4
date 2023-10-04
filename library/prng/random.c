/*
 * $Id: stdlib_random.c,v 1.0 2021-03-22 20:10:26 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->x[-1] = (__clib4->n << 16) | (__clib4->i << 8) | __clib4->j;
    return __clib4->x - 1;
}

void
loadstate(uint32_t *state) {
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->x = state + 1;
    __clib4->n = __clib4->x[-1] >> 16;
    __clib4->i = (__clib4->x[-1] >> 8) & 0xff;
    __clib4->j = __clib4->x[-1] & 0xff;
}

long
random(void) {
    long k;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    ObtainSemaphore(__clib4->__random_lock);

    if (__clib4->n == 0) {
        k = __clib4->x[0] = lcg31(__clib4->x[0]);
        goto end;
    }
    __clib4->x[__clib4->i] += __clib4->x[__clib4->j];
    k = __clib4->x[__clib4->i]>>1;
    if (++__clib4->i == __clib4->n)
        __clib4->i = 0;
    if (++__clib4->j == __clib4->n)
        __clib4->j = 0;

end:
    ReleaseSemaphore(__clib4->__random_lock);

    RETURN(k);
    return k;
}