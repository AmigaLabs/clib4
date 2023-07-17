/*
 * $Id: stdlib_gdtoa-hexnan.c,v 1.0 2023-07-17 14:51:15 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "mprec.h"
#include "gdtoa.h"

static void
L_shift(uint32_t *x, uint32_t *x1, int i) {
    int j;

    i = 8 - i;
    i <<= 2;
    j = ULbits - i;
    do {
        *x |= x[1] << j;
        x[1] >>= i;
    } while (++x < x1);
}

int
hexnan(const char **sp, const struct FPI *fpi, uint32_t *x0) {
    uint32_t c, h, *x, *x1, *xe;
    const char *s;
    int havedig, hd0, i, nbits;

    if (!hexdig['0'])
        hexdig_init();
    nbits = fpi->nbits;
    x = x0 + (nbits >> kshift);
    if (nbits & kmask)
        x++;
    *--x = 0;
    x1 = xe = x;
    havedig = hd0 = i = 0;
    s = *sp;
    while ((c = *(const unsigned char *) ++s)) {
        if (!(h = hexdig[c])) {
            if (c <= ' ') {
                if (hd0 < havedig) {
                    if (x < x1 && i < 8)
                        L_shift(x, x1, i);
                    if (x <= x0) {
                        i = 8;
                        continue;
                    }
                    hd0 = havedig;
                    *--x = 0;
                    x1 = x;
                    i = 0;
                }
                continue;
            }
            if (/*(*/ c == ')' && havedig) {
                *sp = s + 1;
                break;
            }
            return STRTOG_NaN;
        }
        havedig++;
        if (++i > 8) {
            if (x <= x0)
                continue;
            i = 1;
            *--x = 0;
        }
        *x = ((*x << 4) | (h & 0xf));
    }
    if (!havedig)
        return STRTOG_NaN;
    if (x < x1 && i < 8)
        L_shift(x, x1, i);
    if (x > x0) {
        x1 = x0;
        do *x1++ = *x++;
        while (x <= xe);
        do *x1++ = 0;
        while (x1 <= xe);
    } else {
        /* truncate high-order word if necessary */
        if ((i = nbits & (ULbits - 1)) != 0)
            *xe &= ((uint32_t) 0xffffffff) >> (ULbits - i);
    }
    for (x1 = xe;; --x1) {
        if (*x1 != 0)
            break;
        if (x1 == x0) {
            *x1 = 1;
            break;
        }
    }
    return STRTOG_NaNbits;
}