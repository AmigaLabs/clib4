/*
 * $Id: stdlib_arc4random.c,v 1.0 2022-08-22 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#define RANDOMDEV    "RANDOM:"
#define KEYSIZE		128

struct arc4_stream {
    uint8_t i;
    uint8_t j;
    uint8_t s[256];
};

static struct arc4_stream rs = {
        .i = 0,
        .j = 0,
        .s = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
                64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
                96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
                144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
                176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
                192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
                208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
                224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
                240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
        }
};
static int rs_initialized;
static int rs_stired;
static int arc4_count;

static inline uint8_t arc4_getbyte(void);
static void arc4_stir(void);

static struct {
    struct timeval tv;
    pid_t pid;
    uint8_t rnd[KEYSIZE];
} rdat;
static volatile int rs_data_available = 0;

static inline void
arc4_addrandom(u_char *dat, int datlen) {
    int n;
    uint8_t si;

    rs.i--;
    for (n = 0; n < 256; n++) {
        rs.i = (rs.i + 1);
        si = rs.s[rs.i];
        rs.j = (rs.j + si + dat[n % datlen]);
        rs.s[rs.i] = rs.s[rs.j];
        rs.s[rs.j] = si;
    }
    rs.j = rs.i;
}

static void
arc4_fetch(void) {
    int done, fd;
    fd = open(RANDOMDEV, O_RDONLY, 0);
    done = 0;
    if (fd >= 0) {
        if (read(fd, &rdat, KEYSIZE) == KEYSIZE)
            done = 1;
        (void) close(fd);
    }
    if (!done) {
        (void) gettimeofday(&rdat.tv, NULL);
        rdat.pid = getpid();
        /* We'll just take whatever was on the stack too... */
    }
}

static void
arc4_stir(void) {
    int n;
    /*
     * If we don't have data, we need some now before we can integrate
     * it into the static buffers
     */
    if (!rs_data_available) {
        arc4_fetch();
    }
    rs_data_available = 0;
    //__sync_synchronize();

    arc4_addrandom((u_char * ) & rdat, KEYSIZE);

    /*
     * Throw away the first N bytes of output, as suggested in the
     * paper "Weaknesses in the Key Scheduling Algorithm of RC4"
     * by Fluher, Mantin, and Shamir.  N=1024 is based on
     * suggestions in the paper "(Not So) Random Shuffles of RC4"
     * by Ilya Mironov.
     */
    for (n = 0; n < 1024; n++)
        (void) arc4_getbyte();
    arc4_count = 1600000;
    rs_stired = 1;
}

static inline uint8_t

arc4_getbyte(void) {
    uint8_t si, sj;

    rs.i = (rs.i + 1);
    si = rs.s[rs.i];
    rs.j = (rs.j + si);
    sj = rs.s[rs.j];
    rs.s[rs.i] = sj;
    rs.s[rs.j] = si;

    return (rs.s[(si + sj) & 0xff]);
}

static inline uint32_t

arc4_getword(void) {
    uint32_t val;

    val = arc4_getbyte() << 24;
    val |= arc4_getbyte() << 16;
    val |= arc4_getbyte() << 8;
    val |= arc4_getbyte();

    return (val);
}

static inline int
arc4_check_stir(void) {
    if (!rs_stired || arc4_count <= 0) {
        arc4_stir();
        return 1;
    }
    return 0;
}

void
arc4random_stir(void) {
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);
    arc4_stir();
    ReleaseSemaphore(__clib2->__random_lock);
}

void
arc4random_addrandom(u_char *dat, int datlen) {
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);
    arc4_check_stir();
    arc4_addrandom(dat, datlen);
    ReleaseSemaphore(__clib2->__random_lock);
}

uint32_t
arc4random(void) {
    uint32_t rnd;
    struct _clib2 *__clib2 = __CLIB2;

    ObtainSemaphore(__clib2->__random_lock);

    int did_stir = arc4_check_stir();
    rnd = arc4_getword();
    arc4_count -= 4;

    ReleaseSemaphore(__clib2->__random_lock);
    if (did_stir) {
        /* stirring used up our data pool, we need to read in new data outside of the lock */
        arc4_fetch();
        rs_data_available = 1;
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
        if (arc4_check_stir()) {
            did_stir = 1;
        }
        buf[n] = arc4_getbyte();
        arc4_count--;
    }

    ReleaseSemaphore(__clib2->__random_lock);
    if (did_stir) {
        /* stirring used up our data pool, we need to read in new data outside of the lock */
        arc4_fetch();
        rs_data_available = 1;
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