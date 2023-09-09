/*
 * $Id: stdlib_mprec.c,v 1.0 2023-07-17 14:51:15 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H

#include "math_headers.h"

#endif /* _MATH_HEADERS_H */

const double tens[] = {
        1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
        1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
        1e20, 1e21, 1e22, 1e23, 1e24
};

const __float64 bigtens[] = {_F_64(1e16), _F_64(1e32), _F_64(1e64), _F_64(1e128), _F_64(1e256)};
const __float64 tinytens[] = {_F_64(1e-16), _F_64(1e-32), _F_64(1e-64), _F_64(1e-128), _F_64(1e-256)};

double
ulp(double _x) {
    union double_union x, a;
    register int32_t L;

    x.d = _x;

    L = (word0(x) & Exp_mask) - (_P - 1) * Exp_msk1;
#ifndef Sudden_Underflow
    if (L > 0) {
#endif
        word0(a) = L;
        word1(a) = 0;

#ifndef Sudden_Underflow
    } else {
        L = -L >> Exp_shift;
        if (L < Exp_shift) {
            word0(a) = 0x80000 >> L;
            word1(a) = 0;
        } else {
            word0(a) = 0;
            L -= Exp_shift;
            word1(a) = L >= 31 ? 1 : 1 << (31 - L);
        }
    }
#endif
    return a.d;
}

int
cmp(_Bigint *a, _Bigint *b) {
    uint32_t *xa, *xa0, *xb, *xb0;
    int i, j;

    i = a->_wds;
    j = b->_wds;
    if (i -= j)
        return i;
    xa0 = a->_x;
    xa = xa0 + j;
    xb0 = b->_x;
    xb = xb0 + j;
    for (;;) {
        if (*--xa != *--xb)
            return *xa < *xb ? -1 : 1;
        if (xa <= xa0)
            break;
    }
    return 0;
}

_Bigint *
lshift(_Bigint *b, int k) {
    int i, k1, n, n1;
    _Bigint *b1;
    uint32_t *x, *x1, *xe, z;

    n = k >> 5;
    k1 = b->_k;
    n1 = n + b->_wds + 1;
    for (i = b->_maxwds; n1 > i; i <<= 1)
        k1++;
    b1 = Balloc(k1);
    x1 = b1->_x;
    for (i = 0; i < n; i++)
        *x1++ = 0;
    x = b->_x;
    xe = x + b->_wds;
    if (k &= 0x1f) {
        k1 = 32 - k;
        z = 0;
        do {
            *x1++ = *x << k | z;
            z = *x++ >> k1;
        } while (x < xe);
        if ((*x1 = z) != 0)
            ++n1;
    } else
        do
            *x1++ = *x++;
        while (x < xe);
    b1->_wds = n1 - 1;
    Bfree(b);
    return b1;
}

_Bigint *
Balloc(int k) {
    int x;
    _Bigint *rv;

    x = 1 << k;
    /* Allocate an mprec Bigint */
    rv = (_Bigint *) calloc(1, sizeof(_Bigint) + (x - 1) * sizeof(rv->_x));
    if (rv == NULL)
        return NULL;
    rv->_k = k;
    rv->_maxwds = x;
    rv->_sign = rv->_wds = 0;
    return rv;
}

void Bfree(_Bigint *v) {
    free(v);
}

void
copybits(uint32_t *c, int n, _Bigint *b) {
    uint32_t *ce, *x, *xe;

    ce = c + ((n - 1) >> kshift) + 1;
    x = b->_x;
    xe = x + b->_wds;
    while (x < xe)
        *c++ = *x++;
    while (c < ce)
        *c++ = 0;
}

double
ratio(_Bigint *a, _Bigint *b) {
    union double_union da, db;
    int k, ka, kb;

    da.d = b2d(a, &ka);
    db.d = b2d(b, &kb);
    k = ka - kb + 32 * (a->_wds - b->_wds);
    if (k > 0)
        word0(da) += k * Exp_msk1;
    else {
        k = -k;
        word0(db) += k * Exp_msk1;
    }
    return da.d / db.d;
}

_Bigint *
diff(_Bigint *a, _Bigint *b) {
    _Bigint *c;
    int i, wa, wb;
    int32_t borrow, y;        /* We need signed shifts here. */
    uint32_t *xa, *xae, *xb, *xbe, *xc;
    int32_t z;

    i = cmp(a, b);
    if (!i) {
        c = Balloc(0);
        if (!c)
            return NULL;
        c->_wds = 1;
        c->_x[0] = 0;
        return c;
    }
    if (i < 0) {
        c = a;
        a = b;
        b = c;
        i = 1;
    } else
        i = 0;
    c = Balloc(a->_k);
    if (!c)
        return NULL;
    c->_sign = i;
    wa = a->_wds;
    xa = a->_x;
    xae = xa + wa;
    wb = b->_wds;
    xb = b->_x;
    xbe = xb + wb;
    xc = c->_x;
    borrow = 0;

    do {
        y = (*xa & 0xffff) - (*xb & 0xffff) + borrow;
        borrow = y >> 16;
        Sign_Extend(borrow, y);
        z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
        borrow = z >> 16;
        Sign_Extend(borrow, z);
        Storeinc(xc, z, y);
    } while (xb < xbe);
    while (xa < xae) {
        y = (*xa & 0xffff) + borrow;
        borrow = y >> 16;
        Sign_Extend(borrow, y);
        z = (*xa++ >> 16) + borrow;
        borrow = z >> 16;
        Sign_Extend(borrow, z);
        Storeinc(xc, z, y);
    }

    while (!*--xc)
        wa--;
    c->_wds = wa;
    return c;
}

_Bigint *
mult(_Bigint *a, _Bigint *b) {
    _Bigint *c;
    int k, wa, wb, wc;
    uint32_t carry, y, z;
    uint32_t *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
    uint32_t z2;

    if (!a || !b)
        return NULL;

    if (a->_wds < b->_wds) {
        c = a;
        a = b;
        b = c;
    }
    k = a->_k;
    wa = a->_wds;
    wb = b->_wds;
    wc = wa + wb;
    if (wc > a->_maxwds)
        k++;
    c = Balloc(k);
    if (!c)
        return NULL;
    for (x = c->_x, xa = x + wc; x < xa; x++)
        *x = 0;
    xa = a->_x;
    xae = xa + wa;
    xb = b->_x;
    xbe = xb + wb;
    xc0 = c->_x;

    for (; xb < xbe; xb++, xc0++) {
        if ((y = *xb & 0xffff) != 0) {
            x = xa;
            xc = xc0;
            carry = 0;
            do {
                z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
                carry = z >> 16;
                z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
                carry = z2 >> 16;
                Storeinc(xc, z2, z);
            } while (x < xae);
            *xc = carry;
        }
        if ((y = *xb >> 16) != 0) {
            x = xa;
            xc = xc0;
            carry = 0;
            z2 = *xc;
            do {
                z = (*x & 0xffff) * y + (*xc >> 16) + carry;
                carry = z >> 16;
                Storeinc(xc, z, z2);
                z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
                carry = z2 >> 16;
            } while (x < xae);
            *xc = z2;
        }
    }

    for (xc0 = c->_x, xc = xc0 + wc; wc > 0 && !*--xc; --wc);
    c->_wds = wc;
    return c;
}

_Bigint *
pow5mult(_Bigint *b, int k) {
    _Bigint *b1, *p5, *p51;
    int i;
    static const int p05[3] = {5, 25, 125};

    if ((i = k & 3) != 0)
        b = multadd(b, p05[i - 1], 0);

    if (!(k >>= 2))
        return b;
    p5 = i2b(625);
    for (;;) {
        if (k & 1) {
            b1 = mult(b, p5);
            Bfree(b);
            b = b1;
        }
        if (!(k >>= 1))
            break;
        p51 = mult(p5, p5);
        Bfree(p5);
        p5 = p51;
    }
    Bfree(p5);
    return b;
}

_Bigint *
i2b(int i) {
    _Bigint *b;

    b = Balloc(1);
    if (b) {
        b->_x[0] = i;
        b->_wds = 1;
    }
    return b;
}

_Bigint *
s2b(const char *s, int nd0, int nd, uint32_t y9) {
    _Bigint *b;
    int i, k;
    int32_t x, y;

    x = (nd + 8) / 9;
    for (k = 0, y = 1; x > y; y <<= 1, k++);
    b = Balloc(k);
    if (!b)
        return NULL;
    b->_x[0] = y9;
    b->_wds = 1;

    i = 9;
    if (9 < nd0) {
        s += 9;
        do
            b = multadd(b, 10, *s++ - '0');
        while (++i < nd0);
        s++;
    } else
        s += 10;
    for (; i < nd; i++)
        b = multadd(b, 10, *s++ - '0');
    return b;
}

_Bigint *
d2b(double _d, int *e, int *bits) {
    union double_union d;
    _Bigint *b;
    int de, i, k;
    uint32_t *x, y, z;
    d.d = _d;
#define d0 word0(d)
#define d1 word1(d)
    d.d = _d;

    b = Balloc(1);
    x = b->_x;

    z = d0 & Frac_mask;
    d0 &= 0x7fffffff;        /* clear sign bit, which we ignore */
#ifdef Sudden_Underflow
    de = (int) (d0 >> Exp_shift);
#else
    if ((de = (int) (d0 >> Exp_shift)) != 0)
        z |= Exp_msk1;
#endif
    if (d1) {
        y = d1;
        k = lo0bits(&y);
        if (k) {
            x[0] = y | z << (32 - k);
            z >>= k;
        } else
            x[0] = y;
        i = b->_wds = (x[1] = z) ? 2 : 1;
    } else {
        k = lo0bits(&z);
        x[0] = z;
        i = b->_wds = 1;
        k += 32;
    }

#ifndef Sudden_Underflow
    if (de) {
#endif
        *e = de - Bias - (_P - 1) + k;
        *bits = _P - k;
#ifndef Sudden_Underflow
    } else {
        *e = de - Bias - (_P - 1) + 1 + k;
        *bits = 32 * i - hi0bits(x[i - 1]);
    }
#endif
    return b;
}

#undef d0
#undef d1

int
hi0bits(register uint32_t x) {
    register int k = 0;

    if (!(x & 0xffff0000)) {
        k = 16;
        x <<= 16;
    }
    if (!(x & 0xff000000)) {
        k += 8;
        x <<= 8;
    }
    if (!(x & 0xf0000000)) {
        k += 4;
        x <<= 4;
    }
    if (!(x & 0xc0000000)) {
        k += 2;
        x <<= 2;
    }
    if (!(x & 0x80000000)) {
        k++;
        if (!(x & 0x40000000))
            return 32;
    }
    return k;
}


int
lo0bits(uint32_t *y) {
    register int k;
    register uint32_t x = *y;

    if (x & 7) {
        if (x & 1)
            return 0;
        if (x & 2) {
            *y = x >> 1;
            return 1;
        }
        *y = x >> 2;
        return 2;
    }
    k = 0;
    if (!(x & 0xffff)) {
        k = 16;
        x >>= 16;
    }
    if (!(x & 0xff)) {
        k += 8;
        x >>= 8;
    }
    if (!(x & 0xf)) {
        k += 4;
        x >>= 4;
    }
    if (!(x & 0x3)) {
        k += 2;
        x >>= 2;
    }
    if (!(x & 1)) {
        k++;
        x >>= 1;
        if (!(x & 1))
            return 32;
    }
    *y = x;
    return k;
}

_Bigint *
multadd(_Bigint *b, int m, int a) {
    int i, wds;
    uint32_t *x, y;
    uint32_t xi, z;
    _Bigint *b1;

    if (!b)
        return NULL;

    wds = b->_wds;
    x = b->_x;
    i = 0;
    do {
        xi = *x;
        y = (xi & 0xffff) * m + a;
        z = (xi >> 16) * m + (y >> 16);
        a = (int) (z >> 16);
        *x++ = (z << 16) + (y & 0xffff);
    } while (++i < wds);
    if (a) {
        if (wds >= b->_maxwds) {
            b1 = Balloc(b->_k + 1);
            if (!b1) {
                Bfree(b);
                return NULL;
            }
            Bcopy(b1, b);
            Bfree(b);
            b = b1;
        }
        b->_x[wds++] = a;
        b->_wds = wds;
    }
    return b;
}

double
b2d(_Bigint *a, int *e) {
    uint32_t *xa, *xa0, y, z;
    uint32_t w;
    int k;
    union double_union d;
#define d0 word0(d)
#define d1 word1(d)

    xa0 = a->_x;
    xa = xa0 + a->_wds;
    y = *--xa;
    k = hi0bits(y);
    *e = 32 - k;

    if (k < Ebits) {
        d0 = Exp_1 | y >> (Ebits - k);
        w = xa > xa0 ? *--xa : 0;
        d1 = y << ((32 - Ebits) + k) | w >> (Ebits - k);
        goto ret_d;
    }
    z = xa > xa0 ? *--xa : 0;
    if (k -= Ebits) {
        d0 = Exp_1 | y << k | z >> (32 - k);
        y = xa > xa0 ? *--xa : 0;
        d1 = z << k | y >> (32 - k);
    } else {
        d0 = Exp_1 | y;
        d1 = z;
    }
    ret_d:
#undef d0
#undef d1
    return d.d;
}

uint32_t
any_on(_Bigint *b,
       int k) {
    int n, nwds;
    uint32_t *x, *x0, x1, x2;

    x = b->_x;
    nwds = b->_wds;
    n = k >> kshift;
    if (n > nwds)
        n = nwds;
    else if (n < nwds && (k &= kmask)) {
        x1 = x2 = x[n];
        x1 >>= k;
        x1 <<= k;
        if (x1 != x2)
            return 1;
    }
    x0 = x;
    x += n;
    while (x > x0)
        if (*--x)
            return 1;
    return 0;
}