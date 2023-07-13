/*
 * $Id: ndbm_hash_close.c,v 1.0 2023-07-12 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <db.h>
#include "hash.h"
#include "page.h"
#include "extern.h"

#ifdef notdef
static uint32_t hash1(const void *, size_t) __unused;
static uint32_t hash2(const void *, size_t) __unused;
static uint32_t hash3(const void *, size_t) __unused;
#endif

static uint32_t hash4(const void *, size_t);

#ifdef notdef
/*
 * Assume that we've already split the bucket to which this key hashes,
 * calculate that bucket, and check that in fact we did already split it.
 *
 * EJB's original hsearch hash.
 */
#define PRIME1		37
#define PRIME2		1048583

uint32_t
hash1(const void *key, size_t len)
{
    uint32_t h;
    uint8_t *k;

    h = 0;
    k = (uint8_t *)key;
    /* Convert string to integer */
    while (len--)
        h = h * PRIME1 ^ (*k++ - ' ');
    h %= PRIME2;
    return (h);
}

/*
 * Phong Vo's linear congruential hash
 */
#define dcharhash(h, c)	((h) = 0x63c63cd9*(h) + 0x9c39c33d + (c))

uint32_t
hash2(const void *key, size_t len)
{
    uint32_t h;
    uint8_t *e, c, *k;

    k = (uint8_t *)key;
    e = k + len;
    for (h = 0; k != e;) {
        c = *k++;
        if (!c && k > e)
            break;
        dcharhash(h, c);
    }
    return (h);
}

/*
 * This is INCREDIBLY ugly, but fast.  We break the string up into 8 byte
 * units.  On the first time through the loop we get the "leftover bytes"
 * (strlen % 8).  On every other iteration, we perform 8 HASHC's so we handle
 * all 8 bytes.  Essentially, this saves us 7 cmp & branch instructions.  If
 * this routine is heavily used enough, it's worth the ugly coding.
 *
 * Ozan Yigit's original sdbm hash.
 */
uint32_t
hash3(const void *key, size_t len)
{
    uint32_t n, loop;
    uint8_t *k;

#define HASHC   n = *k++ + 65599 * n

    n = 0;
    k = (uint8_t *)key;
    if (len > 0) {
        loop = (len + 8 - 1) >> 3;

        switch (len & (8 - 1)) {
        case 0:
            do {	/* All fall throughs */
                HASHC;
        case 7:
                HASHC;
        case 6:
                HASHC;
        case 5:
                HASHC;
        case 4:
                HASHC;
        case 3:
                HASHC;
        case 2:
                HASHC;
        case 1:
                HASHC;
            } while (--loop);
        }

    }
    return (n);
}
#endif /* notdef */

/* Chris Torek's hash function. */
uint32_t
hash4(const void *key, size_t len) {
    uint32_t h, loop;
    const uint8_t *k;

#define HASH4a   h = (h << 5) - h + *k++;
#define HASH4b   h = (h << 5) + h + *k++;
#define HASH4 HASH4b

    h = 0;
    k = key;
    if (len > 0) {
        loop = (len + 8 - 1) >> 3;

        switch (len & (8 - 1)) {
            case 0:
                do {    /* All fall throughs */
                    HASH4;
                    // fall through
                    case 7:
                        HASH4;
                        // fall through
                    case 6:
                        HASH4;
                        // fall through
                    case 5:
                        HASH4;
                        // fall through
                    case 4:
                        HASH4;
                        // fall through
                    case 3:
                        HASH4;
                        // fall through
                    case 2:
                        HASH4;
                        // fall through
                    case 1:
                        HASH4;
                        // fall through
                } while (--loop);
        }

    }
    return (h);
}