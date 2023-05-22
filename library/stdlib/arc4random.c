/*
 * $Id: stdlib_arc4random.c,v 1.0 2022-08-22 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

static inline uint8_t arc4_getbyte(struct _clib2 *__clib2);
static void arc4_stir(struct _clib2 *__clib2);

static inline void
arc4_addrandom(struct _clib2 *__clib2, u_char *dat, int datlen) {
    int n;
    uint8_t si;

    __clib2->rs.i--;
    for (n = 0; n < 256; n++) {
        __clib2->rs.i = (__clib2->rs.i + 1);
        si = __clib2->rs.s[__clib2->rs.i];
        __clib2->rs.j = (__clib2->rs.j + si + dat[n % datlen]);
        __clib2->rs.s[__clib2->rs.i] = __clib2->rs.s[__clib2->rs.j];
        __clib2->rs.s[__clib2->rs.j] = si;
    }
    __clib2->rs.j = __clib2->rs.i;
}

static void
arc4_fetch(struct _clib2 *__clib2) {
    int done, fd;
    fd = open(RANDOMDEV, O_RDONLY, 0);
    done = 0;
    if (fd >= 0) {
        if (read(fd, &__clib2->rdat, KEYSIZE) == KEYSIZE)
            done = 1;
        (void) close(fd);
    }
    if (!done) {
        (void) gettimeofday(&__clib2->rdat.tv, NULL);
        __clib2->rdat.pid = getpid();
        /* We'll just take whatever was on the stack too... */
    }
}

static void
arc4_stir(struct _clib2 *__clib2) {
    int n;

    /*
     * If we don't have data, we need some now before we can integrate
     * it into the static buffers
     */
    if (!__clib2->rs_data_available) {
        arc4_fetch(__clib2);
    }
    __clib2->rs_data_available = 0;
    //__sync_synchronize();

    arc4_addrandom(__clib2, (u_char * ) & __clib2->rdat, KEYSIZE);

    /*
     * Throw away the first N bytes of output, as suggested in the
     * paper "Weaknesses in the Key Scheduling Algorithm of RC4"
     * by Fluher, Mantin, and Shamir.  N=1024 is based on
     * suggestions in the paper "(Not So) Random Shuffles of RC4"
     * by Ilya Mironov.
     */
    for (n = 0; n < 1024; n++)
        (void) arc4_getbyte(__clib2);
    __clib2->arc4_count = 1600000;
    __clib2->rs_stired = 1;
}

static inline uint8_t
arc4_getbyte(struct _clib2 *__clib2) {
    uint8_t si, sj;

    __clib2->rs.i = (__clib2->rs.i + 1);
    si = __clib2->rs.s[__clib2->rs.i];
    __clib2->rs.j = (__clib2->rs.j + si);
    sj = __clib2->rs.s[__clib2->rs.j];
    __clib2->rs.s[__clib2->rs.i] = sj;
    __clib2->rs.s[__clib2->rs.j] = si;

    return (__clib2->rs.s[(si + sj) & 0xff]);
}

static inline uint32_t
arc4_getword(struct _clib2 *__clib2) {
    uint32_t val;

    val = arc4_getbyte(__clib2) << 24;
    val |= arc4_getbyte(__clib2) << 16;
    val |= arc4_getbyte(__clib2) << 8;
    val |= arc4_getbyte(__clib2);

    return (val);
}

static inline int
arc4_check_stir(struct _clib2 *__clib2) {
    if (!__clib2->rs_stired || __clib2->arc4_count <= 0) {
        arc4_stir(__clib2);
        return 1;
    }
    return 0;
}

void
arc4random_stir(void) {
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);
    arc4_stir(__clib2);
    ReleaseSemaphore(__clib2->__random_lock);
}

void
arc4random_addrandom(u_char *dat, int datlen) {
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);
    arc4_check_stir(__clib2);
    arc4_addrandom(__clib2, dat, datlen);
    ReleaseSemaphore(__clib2->__random_lock);
}

uint32_t
arc4random(void) {
    uint32_t rnd;
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);

    int did_stir = arc4_check_stir(__clib2);
    rnd = arc4_getword(__clib2);
    __clib2->arc4_count -= 4;

    ReleaseSemaphore(__clib2->__random_lock);
    if (did_stir) {
        /* stirring used up our data pool, we need to read in new data outside of the lock */
        arc4_fetch(__clib2);
        __clib2->rs_data_available = 1;
        __sync_synchronize();
    }

    return (rnd);
}

void
arc4random_buf(void *_buf, size_t n) {
    u_char *buf = (u_char *) _buf;
    int did_stir = 0;
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);

    while (n--) {
        if (arc4_check_stir(__clib2)) {
            did_stir = 1;
        }
        buf[n] = arc4_getbyte(__clib2);
        __clib2->arc4_count--;
    }

    ReleaseSemaphore(__clib2->__random_lock);
    if (did_stir) {
        /* stirring used up our data pool, we need to read in new data outside of the lock */
        arc4_fetch(__clib2);
        __clib2->rs_data_available = 1;
        __sync_synchronize();
    }
}

/*
 * Calculate a uniformly distributed random number less than upper_bound
 * avoiding "modulo bias".
 *
 * Uniformity is achieved by generating new random numbers until the one
 * returned is outside the range [0, 2**32 % upper_bound).  This
 * guarantees the selected random number will be inside
 * [2**32 % upper_bound, 2**32) which maps back to [0, upper_bound)
 * after reduction modulo upper_bound.
 */
uint32_t
arc4random_uniform(uint32_t upper_bound) {
    uint32_t r, min;

    if (upper_bound < 2)
        return (0);

#if (ULONG_MAX > 0xffffffffUL)
    min = 0x100000000UL % upper_bound;
#else
    /* Calculate (2**32 % upper_bound) avoiding 64-bit math */
    if (upper_bound > 0x80000000)
        min = 1 + ~upper_bound;        /* 2**32 - upper_bound */
    else {
        /* (2**32 - (x * 2)) % x == 2**32 % x when x <= 2**31 */
        min = ((0xffffffff - (upper_bound * 2)) + 1) % upper_bound;
    }
#endif

    /*
     * This could theoretically loop forever but each retry has
     * p > 0.5 (worst case, usually far better) of selecting a
     * number inside the range we need, so it should rarely need
     * to re-roll.
     */
    for (;;) {
        r = arc4random();
        if (r >= min)
            break;
    }

    return (r % upper_bound);
}