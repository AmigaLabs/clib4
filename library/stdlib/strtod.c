/*
 * $Id: stdlib_strtod.c,v 1.12 2006-09-25 14:51:15 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#define Avoid_Underflow
#define Rounding rounding
#define Check_FLT_ROUNDS
#define INFNAN_CHECK
#define IEEE_Arith
#define Honor_FLT_ROUNDS

#define _0 0
#define _1 1

#undef SI
#ifdef Sudden_Underflow
#define SI 1
#else
#define SI 0
#endif

static double
sulp(U x, int scale) {
    U u;
    double rv;
    int i;

    rv = ulp(dval(x));
    if (!scale || (i = 2 * _P + 1 - ((dword0(x) & Exp_mask) >> Exp_shift)) <= 0)
        return rv; /* Is there an example where i <= 0 ? */
    dword0(u) = Exp_1 + ((int32_t) i << Exp_shift);
    dword1(u) = 0;
    return rv * u.d;
}

static int
match (const char **sp, char *t) {
    int c, d;
    const char *s = *sp;

    while( (d = *t++) !=0) {
        if ((c = *++s) >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        if (c != d)
            return 0;
    }
    *sp = s + 1;
    return 1;
}

static void
ULtod(uint32_t *L, uint32_t *bits, int32_t exp, int k) {
    switch (k & STRTOG_Retmask) {
        case STRTOG_NoNumber:
        case STRTOG_Zero:
            L[0] = L[1] = 0;
            break;

        case STRTOG_Denormal:
            L[_1] = bits[0];
            L[_0] = bits[1];
            break;

        case STRTOG_Normal:
        case STRTOG_NaNbits:
            L[_1] = bits[0];
            L[_0] = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
            break;

        case STRTOG_Infinite:
            L[_0] = 0x7ff00000;
            L[_1] = 0;
            break;

        case STRTOG_NaN:
            L[_0] = 0x7fffffff;
            L[_1] = (uint32_t) - 1;
    }
    if (k & STRTOG_Neg)
        L[_0] |= 0x80000000L;
}

double
strtod_l(const char *__restrict s00, char **__restrict se, locale_t loc) {
#ifdef Avoid_Underflow
    int scale;
#endif
    int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, dsign,
            e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
    const char *s, *s0, *s1;
    double aadj, adj;
    U aadj1, rv, rv0;
    int32_t L;
    uint32_t y, z;
    _Bigint *bb = NULL, *bb1, *bd = NULL, *bd0, *bs = NULL, *delta = NULL;
#ifdef Avoid_Underflow
    uint32_t Lsb, Lsb1;
#endif
#ifdef Honor_FLT_ROUNDS
    int rounding;
#endif
    struct _clib2 *__clib2 = __CLIB2;
    char *decimal_point = ".";
    int dec_len = 1;
    if (__clib2->__locale_table[LC_NUMERIC] != NULL) {
        decimal_point = __clib2->__locale_table[LC_NUMERIC]->loc_DecimalPoint;
        dec_len = strlen(decimal_point);
    }

    delta = bs = bd = NULL;
    sign = nz0 = nz = decpt = 0;
    dval(rv) = 0.;
    for (s = s00;; s++)
        switch (*s) {
            case '-':
                sign = 1;
                /* Fallthrough */
            case '+':
                if (*++s)
                    goto break2;
                /* Fallthrough */
            case 0:
                goto ret0;
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
            case ' ':
                continue;
            default:
                goto break2;
        }
    break2:
    if (*s == '0') {
        {
            static const FPI fpi = {53, 1 - 1023 - 53 + 1, 2046 - 1023 - 53 + 1, 1, SI};
            int32_t exp;
            uint32_t bits[2];
            switch (s[1]) {
                case 'x':
                case 'X':
                    /* If the number is not hex, then the parse of
                                   0 is still valid.  */
                    s00 = s + 1;
                    {
                        FPI fpi1 = fpi;
                        switch (fegetround()) {
                            case FE_TOWARDZERO:
                                fpi1.rounding = 0;
                                break;
                            case FE_UPWARD:
                                fpi1.rounding = 2;
                                break;
                            case FE_DOWNWARD:
                                fpi1.rounding = 3;
                        }

                        switch ((i = gethex(&s, &fpi1, &exp, &bb, sign, loc)) & STRTOG_Retmask) {
                            case STRTOG_NoNumber:
                                s = s00;
                                sign = 0;
                                /* Fallthrough */
                            case STRTOG_Zero:
                                break;
                            default:
                                if (bb) {
                                    copybits(bits, fpi.nbits, bb);
                                    Bfree(bb);
                                }
                                ULtod(rv.i, bits, exp, i);
#ifndef NO_ERRNO
                                /* try to avoid the bug of testing an 8087 register value */
                                if ((dword0(rv) & Exp_mask) == 0)
                                    errno = ERANGE;
#endif
                        }
                    }
                    goto ret;
            }
        }

        nz0 = 1;
        while (*++s == '0');
        if (!*s)
            goto ret;
    }
    s0 = s;
    y = z = 0;
    for (nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
        if (nd < 9)
            y = 10 * y + c - '0';
        else
            z = 10 * z + c - '0';
    nd0 = nd;
    if (strncmp(s, decimal_point, dec_len) == 0) {
        decpt = 1;
        c = *(s += dec_len);
        if (!nd) {
            for (; c == '0'; c = *++s)
                nz++;
            if (c > '0' && c <= '9') {
                s0 = s;
                nf += nz;
                nz = 0;
                goto have_dig;
            }
            goto dig_done;
        }
        for (; c >= '0' && c <= '9'; c = *++s) {
            have_dig:
            nz++;
            if (c -= '0') {
                nf += nz;
                for (i = 1; i < nz; i++)
                    if (nd++ < 9)
                        y *= 10;
                    else if (nd <= DBL_DIG + 1)
                        z *= 10;
                if (nd++ < 9)
                    y = 10 * y + c;
                else if (nd <= DBL_DIG + 1)
                    z = 10 * z + c;
                nz = 0;
            }
        }
    }
    dig_done:
    e = 0;
    if (c == 'e' || c == 'E') {
        if (!nd && !nz && !nz0) {
            goto ret0;
        }
        s00 = s;
        esign = 0;
        switch (c = *++s) {
            case '-':
                esign = 1;
                /* Fallthrough */
            case '+':
                c = *++s;
        }
        if (c >= '0' && c <= '9') {
            while (c == '0')
                c = *++s;
            if (c > '0' && c <= '9') {
                L = c - '0';
                s1 = s;
                while ((c = *++s) >= '0' && c <= '9')
                    L = 10 * L + c - '0';
                if (s - s1 > 8 || L > 19999)
                    /* Avoid confusion from exponents
                     * so large that e might overflow.
                     */
                    e = 19999; /* safe for 16 bit ints */
                else
                    e = (int) L;
                if (esign)
                    e = -e;
            } else
                e = 0;
        } else
            s = s00;
    }
    if (!nd) {
        if (!nz && !nz0) {
#ifdef INFNAN_CHECK
            /* Check for Nan and Infinity */
            uint32_t bits[2];
            static const FPI fpinan =    /* only 52 explicit bits */
                    {52, 1 - 1023 - 53 + 1, 2046 - 1023 - 53 + 1, 1, SI};
            if (!decpt)
                switch (c) {
                    case 'i':
                    case 'I':
                        if (match(&s, "nf")) {
                            --s;
                            if (!match(&s, "inity"))
                                ++s;
                            dword0(rv) = 0x7ff00000;
                            dword1(rv) = 0;
                            goto ret;
                        }
                        break;
                    case 'n':
                    case 'N':
                        if (match(&s, "an")) {
                            if (*s == '(' /*)*/
                                && hexnan(&s, &fpinan, bits)
                                   == STRTOG_NaNbits) {
                                dword0(rv) = 0x7ff00000 | bits[1];
                                dword1(rv) = bits[0];
                            } else {
                                dval(rv) = nan("");
                            }
                            goto ret;
                        }
                }
#endif /* INFNAN_CHECK */
            ret0:
            s = s00;
            sign = 0;
        }
        goto ret;
    }
    e1 = e -= nf;

    /* Now we have nd0 digits, starting at s0, followed by a
     * decimal point, followed by nd-nd0 digits.  The number we're
     * after is the integer represented by those digits times
     * 10**e */

    if (!nd0)
        nd0 = nd;
    k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
    dval(rv) = y;
    if (k > 9) {
#ifdef SET_INEXACT
        if (k > DBL_DIG)
            oldinexact = get_inexact();
#endif
        dval(rv) = tens[k - 9] * dval(rv) + z;
    }
    bd0 = 0;
    if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
#ifndef Honor_FLT_ROUNDS
        && Flt_Rounds == 1
#endif
#endif
            ) {
        if (!e)
            goto ret;
        if (e > 0) {
            if (e <= Ten_pmax) {
#ifdef Honor_FLT_ROUNDS
                /* round correctly FLT_ROUNDS = 2 or 3 */
                if (sign) {
                    dval(rv) = -dval(rv);
                    sign = 0;
                }
#endif
                /* rv = */ rounded_product(dval(rv), tens[e]);
                goto ret;
            }
            i = DBL_DIG - nd;
            if (e <= Ten_pmax + i) {
                /* A fancier test would sometimes let us do
                 * this for larger i values.
                 */
#ifdef Honor_FLT_ROUNDS
                /* round correctly FLT_ROUNDS = 2 or 3 */
                if (sign) {
                    dval(rv) = -dval(rv);
                    sign = 0;
                }
#endif
                e -= i;
                dval(rv) *= tens[i];
                /* rv = */ rounded_product(dval(rv), tens[e]);
                goto ret;
            }
        }
#ifndef Inaccurate_Divide
        else if (e >= -Ten_pmax) {
#ifdef Honor_FLT_ROUNDS
            /* round correctly FLT_ROUNDS = 2 or 3 */
            if (sign) {
                dval(rv) = -dval(rv);
                sign = 0;
            }
#endif
            /* rv = */ rounded_quotient(dval(rv), tens[-e]);
            goto ret;
        }
#endif
    }
    e1 += nd - k;

#ifdef IEEE_Arith
#ifdef Avoid_Underflow
    scale = 0;
#endif
#ifdef Honor_FLT_ROUNDS
    if ((rounding = Flt_Rounds) >= 2) {
        if (sign)
            rounding = rounding == 2 ? 0 : 2;
        else if (rounding != 2)
            rounding = 0;
    }
#endif
#endif /*IEEE_Arith*/

    /* Get starting approximation = rv * 10**e1 */

    if (e1 > 0) {
        if ((i = e1 & 15) != 0)
            dval(rv) *= tens[i];
        if (e1 &= ~15) {
            if (e1 > DBL_MAX_10_EXP) {
                ovfl:
#ifndef NO_ERRNO
                __set_errno(ERANGE);
#endif
                /* Can't trust HUGE_VAL */
#ifdef IEEE_Arith
#ifdef Honor_FLT_ROUNDS
                switch (rounding) {
                    case 0: /* toward 0 */
                    case 3: /* toward -infinity */
                        dword0(rv) = Big0;
                        dword1(rv) = Big1;
                        break;
                    default:
                        dword0(rv) = Exp_mask;
                        dword1(rv) = 0;
                }
#else /*Honor_FLT_ROUNDS*/
                dword0(rv) = Exp_mask;
                dword1(rv) = 0;
#endif /*Honor_FLT_ROUNDS*/
#else /*IEEE_Arith*/
                dword0(rv) = Big0;
                dword1(rv) = Big1;
#endif /*IEEE_Arith*/
                if (bd0)
                    goto retfree;
                goto ret;
            }
            e1 >>= 4;
            for (j = 0; e1 > 1; j++, e1 >>= 1)
                if (e1 & 1)
                    dval(rv) *= bigtens[j];
            /* The last multiplication could overflow. */
            dword0(rv) -= _P * Exp_msk1;
            dval(rv) *= bigtens[j];
            if ((z = dword0(rv) & Exp_mask)
                > Exp_msk1 * (DBL_MAX_EXP + Bias - _P))
                goto ovfl;
            if (z > Exp_msk1 * (DBL_MAX_EXP + Bias - 1 - _P)) {
                /* set to largest number */
                /* (Can't trust DBL_MAX) */
                dword0(rv) = Big0;
                dword1(rv) = Big1;
            } else
                dword0(rv) += _P * Exp_msk1;
        }
    } else if (e1 < 0) {
        e1 = -e1;
        if ((i = e1 & 15) != 0)
            dval(rv) /= tens[i];
        if (e1 >>= 4) {
            if (e1 >= 1 << n_bigtens)
                goto undfl;
#ifdef Avoid_Underflow
            if (e1 & Scale_Bit)
                scale = 2 * _P;
            for (j = 0; e1 > 0; j++, e1 >>= 1)
                if (e1 & 1)
                    dval(rv) *= tinytens[j];
            if (scale && (j = 2 * _P + 1 - ((dword0(rv) & Exp_mask)
                    >> Exp_shift)) > 0) {
                /* scaled rv is denormal; zap j low bits */
                if (j >= 32) {
                    dword1(rv) = 0;
                    if (j >= 53)
                        dword0(rv) = (_P + 2) * Exp_msk1;
                    else
                        dword0(rv) &= 0xffffffff << (j - 32);
                } else
                    dword1(rv) &= 0xffffffff << j;
            }
#else
            for (j = 0; e1 > 1; j++, e1 >>= 1)
                if (e1 & 1)
                    dval(rv) *= tinytens[j];
            /* The last multiplication could underflow. */
            dval(rv0) = dval(rv);
            dval(rv) *= tinytens[j];
            if (!dval(rv)) {
                dval(rv) = 2. * dval(rv0);
                dval(rv) *= tinytens[j];
#endif
            if (!dval(rv)) {
                undfl:
                dval(rv) = 0.;
#ifndef NO_ERRNO
                __set_errno(ERANGE);
#endif
                if (bd0)
                    goto retfree;
                goto ret;
            }
#ifndef Avoid_Underflow
            dword0(rv) = Tiny0;
            dword1(rv) = Tiny1;
            /* The refinement below will clean
             * this approximation up.
             */
        }
#endif
        }
    }

    /* Now the hard part -- adjusting rv to the correct value.*/

    /* Put digits into bd: true value = bd * 10^e */

    bd0 = s2b(s0, nd0, nd, y);
    if (bd0 == NULL)
        goto ovfl;

    for (;;) {
        bd = Balloc(bd0->_k);
        if (bd == NULL)
            goto ovfl;
        Bcopy(bd, bd0);
        bb = d2b(dval(rv), &bbe, &bbbits);    /* rv = bb * 2^bbe */
        if (bb == NULL)
            goto ovfl;
        bs = i2b(1);
        if (bs == NULL)
            goto ovfl;

        if (e >= 0) {
            bb2 = bb5 = 0;
            bd2 = bd5 = e;
        } else {
            bb2 = bb5 = -e;
            bd2 = bd5 = 0;
        }
        if (bbe >= 0)
            bb2 += bbe;
        else
            bd2 -= bbe;
        bs2 = bb2;
#ifdef Honor_FLT_ROUNDS
        if (rounding != 1)
            bs2++;
#endif
#ifdef Avoid_Underflow
        Lsb = LSB;
        Lsb1 = 0;
        j = bbe - scale;
        i = j + bbbits - 1;    /* logb(rv) */
        j = _P + 1 - bbbits;
        if (i < Emin) {    /* denormal */
            i = Emin - i;
            j -= i;
            if (i < 32)
                Lsb <<= i;
            else
                Lsb1 = Lsb << (i - 32);
        }
#else /*Avoid_Underflow*/
#ifdef Sudden_Underflow
        j = _P + 1 - bbbits;
#else /*Sudden_Underflow*/
        j = bbe;
        i = j + bbbits - 1;    /* logb(rv) */
        if (i < Emin)    /* denormal */
            j += _P - Emin;
        else
            j = _P + 1 - bbbits;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
        bb2 += j;
        bd2 += j;
#ifdef Avoid_Underflow
        bd2 += scale;
#endif
        i = bb2 < bd2 ? bb2 : bd2;
        if (i > bs2)
            i = bs2;
        if (i > 0) {
            bb2 -= i;
            bd2 -= i;
            bs2 -= i;
        }
        if (bb5 > 0) {
            bs = pow5mult(bs, bb5);
            if (bs == NULL)
                goto ovfl;
            bb1 = mult(bs, bb);
            if (bb1 == NULL)
                goto ovfl;
            Bfree(bb);
            bb = bb1;
        }
        if (bb2 > 0) {
            bb = lshift(bb, bb2);
            if (bb == NULL)
                goto ovfl;
        }
        if (bd5 > 0) {
            bd = pow5mult(bd, bd5);
            if (bd == NULL)
                goto ovfl;
        }
        if (bd2 > 0) {
            bd = lshift(bd, bd2);
            if (bd == NULL)
                goto ovfl;
        }
        if (bs2 > 0) {
            bs = lshift(bs, bs2);
            if (bs == NULL)
                goto ovfl;
        }
        delta = diff(bb, bd);
        if (delta == NULL)
            goto ovfl;
        dsign = delta->_sign;
        delta->_sign = 0;
        i = cmp(delta, bs);
#ifdef Honor_FLT_ROUNDS
        if (rounding != 1) {
            if (i < 0) {
                /* Error is less than an ulp */
                if (!delta->_x[0] && delta->_wds <= 1) {
                    /* exact */
                    break;
                }
                if (rounding) {
                    if (dsign) {
                        adj = 1.;
                        goto apply_adj;
                    }
                } else if (!dsign) {
                    adj = -1.;
                    if (!dword1(rv)
                        && !(dword0(rv) & Frac_mask)) {
                        y = dword0(rv) & Exp_mask;
#ifdef Avoid_Underflow
                        if (!scale || y > 2 * _P * Exp_msk1)
#else
                            if (y)
#endif
                        {
                            delta = lshift(delta, Log2P);
                            if (cmp(delta, bs) <= 0)
                                adj = -0.5;
                        }
                    }
                    apply_adj:
#ifdef Avoid_Underflow
                    if (scale && (y = dword0(rv) & Exp_mask)
                                 <= 2 * _P * Exp_msk1)
                        adj += (2 * _P + 1) * Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
                    if ((dword0(rv) & Exp_mask) <=
                            _P*Exp_msk1) {
                        dword0(rv) += _P*Exp_msk1;
                        dval(rv) += adj*ulp(dval(rv));
                        dword0(rv) -= _P*Exp_msk1;
                        }
                    else
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
                    dval(rv) += adj * ulp(dval(rv));
                }
                break;
            }
            adj = ratio(delta, bs);
            if (adj < (double) 1.)
                adj = (double) 1.;
            if (adj <= 0x7ffffffe) {
                /* adj = rounding ? ceil(adj) : floor(adj); */
                y = adj;
                if (y != adj) {
                    if (!((rounding >> 1) ^ dsign))
                        y++;
                    adj = y;
                }
            }
#ifdef Avoid_Underflow
            if (scale && (y = dword0(rv) & Exp_mask) <= 2 * _P * Exp_msk1)
                adj += (2 * _P + 1) * Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
            if ((dword0(rv) & Exp_mask) <= _P*Exp_msk1) {
                dword0(rv) += _P*Exp_msk1;
                adj *= ulp(dval(rv));
                if (dsign)
                    dval(rv) += adj;
                else
                    dval(rv) -= adj;
                dword0(rv) -= _P*Exp_msk1;
                goto cont;
                }
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
            adj *= ulp(dval(rv));
            if (dsign)
                dval(rv) += adj;
            else
                dval(rv) -= adj;
            goto cont;
        }
#endif /*Honor_FLT_ROUNDS*/

        if (i < 0) {
            /* Error is less than half an ulp -- check for
             * special case of mantissa a power of two.
             */
            if (dsign || dword1(rv) || dword0(rv) & Bndry_mask
                #ifdef IEEE_Arith
                #ifdef Avoid_Underflow
                || (dword0(rv) & Exp_mask) <= (2 * _P + 1) * Exp_msk1
#else
                || (dword0(rv) & Exp_mask) <= Exp_msk1
#endif
#endif
                    ) {
                break;
            }
            if (!delta->_x[0] && delta->_wds <= 1) {
                /* exact result */
                break;
            }
            delta = lshift(delta, Log2P);
            if (cmp(delta, bs) > 0)
                goto drop_down;
            break;
        }
        if (i == 0) {
            /* exactly half-way between */
            if (dsign) {
                if ((dword0(rv) & Bndry_mask1) == Bndry_mask1
                    && dword1(rv) == (
#ifdef Avoid_Underflow
                        (scale && (y = dword0(rv) & Exp_mask) <= 2 * _P * Exp_msk1)
                        ? (0xffffffff & (0xffffffff << (2 * _P + 1 - (y >> Exp_shift)))) :
                        #endif
                        0xffffffff)) {
                    /*boundary case -- increment exponent*/
                    if (dword0(rv) == Big0 && dword1(rv) == Big1)
                        goto ovfl;
                    dword0(rv) = (dword0(rv) & Exp_mask)
                                 + Exp_msk1;
                    dword1(rv) = 0;
#ifdef Avoid_Underflow
                    dsign = 0;
#endif
                    break;
                }
            } else if (!(dword0(rv) & Bndry_mask) && !dword1(rv)) {
                drop_down:
                /* boundary case -- decrement exponent */
#ifdef Sudden_Underflow /*{{*/
                L = dword0(rv) & Exp_mask;
#ifdef Avoid_Underflow
                if (L <= (scale ? (2*_P+1)*Exp_msk1 : Exp_msk1))
#else
                if (L <= Exp_msk1)
#endif /*Avoid_Underflow*/
                    goto undfl;
                L -= Exp_msk1;
#else /*Sudden_Underflow}{*/
#ifdef Avoid_Underflow
                if (scale) {
                    L = dword0(rv) & Exp_mask;
                    if (L <= (int32_t)((2 * _P + 1) * Exp_msk1)) {
                        if (L > (int32_t)((_P + 2) * Exp_msk1))
                            /* round even ==> */
                            /* accept rv */
                            break;
                        /* rv = smallest denormal */
                        goto undfl;
                    }
                }
#endif /*Avoid_Underflow*/
                L = (dword0(rv) & Exp_mask) - Exp_msk1;
#endif /*Sudden_Underflow}*/
                dword0(rv) = L | Bndry_mask1;
                dword1(rv) = 0xffffffff;
                break;
            }
#ifndef ROUND_BIASED
#ifdef Avoid_Underflow
            if (Lsb1) {
                if (!(dword0(rv) & Lsb1))
                    break;
            } else if (!(dword1(rv) & Lsb))
                break;
#else
            if (!(dword1(rv) & LSB))
                break;
#endif
#endif
            if (dsign)
#ifdef Avoid_Underflow
                dval(rv) += sulp(rv, scale);
#else
                dval(rv) += ulp(dval(rv));
#endif
#ifndef ROUND_BIASED
            else {
#ifdef Avoid_Underflow
                dval(rv) -= sulp(rv, scale);
#else
                dval(rv) -= ulp(dval(rv));
#endif
#ifndef Sudden_Underflow
                if (!dval(rv))
                    goto undfl;
#endif
            }
#ifdef Avoid_Underflow
            dsign = 1 - dsign;
#endif
#endif
            break;
        }
        if ((aadj = ratio(delta, bs)) <= (double) 2.) {
            if (dsign)
                aadj = dval(aadj1) = (double) 1.;
            else if (dword1(rv) || dword0(rv) & Bndry_mask) {
#ifndef Sudden_Underflow
                if (dword1(rv) == Tiny1 && !dword0(rv))
                    goto undfl;
#endif
                aadj = (double) 1.;
                dval(aadj1) = (double) -1.;
            } else {
                /* special case -- power of FLT_RADIX to be */
                /* rounded down... */

                if (aadj < (double) 2. / FLT_RADIX)
                    aadj = (double) 1. / FLT_RADIX;
                else
                    aadj *= (double) 0.5;
                dval(aadj1) = -aadj;
            }
        } else {
            aadj *= (double) 0.5;
            dval(aadj1) = dsign ? aadj : -aadj;
#ifdef Check_FLT_ROUNDS
            switch (Rounding) {
                case 2: /* towards +infinity */
                    dval(aadj1) -= (double) 0.5;
                    break;
                case 0: /* towards 0 */
                case 3: /* towards -infinity */
                    dval(aadj1) += (double) 0.5;
            }
#else
            if (Flt_Rounds == 0)
                dval(aadj1) += (double) 0.5;
#endif /*Check_FLT_ROUNDS*/
        }
        y = dword0(rv) & Exp_mask;

        /* Check for overflow */

        if (y == Exp_msk1 * (DBL_MAX_EXP + Bias - 1)) {
            dval(rv0) = dval(rv);
            dword0(rv) -= _P * Exp_msk1;
            adj = dval(aadj1) * ulp(dval(rv));
            dval(rv) += adj;
            if ((dword0(rv) & Exp_mask) >=
                Exp_msk1 * (DBL_MAX_EXP + Bias - _P)) {
                if (dword0(rv0) == Big0 && dword1(rv0) == Big1)
                    goto ovfl;
                dword0(rv) = Big0;
                dword1(rv) = Big1;
                goto cont;
            } else
                dword0(rv) += _P * Exp_msk1;
        } else {
#ifdef Avoid_Underflow
            if (scale && y <= 2 * _P * Exp_msk1) {
                if (aadj <= 0x7fffffff) {
                    if ((z = aadj) == 0)
                        z = 1;
                    aadj = z;
                    dval(aadj1) = dsign ? aadj : -aadj;
                }
                dword0(aadj1) += (2 * _P + 1) * Exp_msk1 - y;
            }
            adj = dval(aadj1) * ulp(dval(rv));
            dval(rv) += adj;
#else
#ifdef Sudden_Underflow
            if ((dword0(rv) & Exp_mask) <= _P*Exp_msk1) {
                dval(rv0) = dval(rv);
                dword0(rv) += _P*Exp_msk1;
                adj = dval(aadj1) * ulp(dval(rv));
                dval(rv) += adj;
                if ((dword0(rv) & Exp_mask) <= _P*Exp_msk1)
                    {
                    if (dword0(rv0) == Tiny0
                     && dword1(rv0) == Tiny1)
                        goto undfl;
                    dword0(rv) = Tiny0;
                    dword1(rv) = Tiny1;
                    goto cont;
                    }
                else
                    dword0(rv) -= _P*Exp_msk1;
                }
            else {
                adj = dval(aadj1) * ulp(dval(rv));
                dval(rv) += adj;
                }
#else /*Sudden_Underflow*/
            /* Compute adj so that the IEEE rounding rules will
             * correctly round rv + adj in some half-way cases.
             * If rv * ulp(rv) is denormalized (i.e.,
             * y <= (_P-1)*Exp_msk1), we must adjust aadj to avoid
             * trouble from bits lost to denormalization;
             * example: 1.2e-307 .
             */
            if (y <= (_P - 1) * Exp_msk1 && aadj > 1.) {
                dval(aadj1) = (double) (int) (aadj + 0.5);
                if (!dsign)
                    dval(aadj1) = -dval(aadj1);
            }
            adj = dval(aadj1) * ulp(dval(rv));
            dval(rv) += adj;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
        }
        z = dword0(rv) & Exp_mask;
#ifdef Avoid_Underflow
        if (!scale)
#endif
            if (y == z) {
                /* Can we stop now? */
                /* If FE_INVALID floating point exceptions are
                   enabled, a conversion to a 32 bit value is
                   dangerous.  A positive double value can result
                   in a negative 32 bit int, thus raising SIGFPE.
                   To avoid this, always convert into 64 bit here. */
                int64_t L = (int64_t) aadj;
                aadj -= L;
                /* The tolerances below are conservative. */
                if (dsign || dword1(rv) || dword0(rv) & Bndry_mask) {
                    if (aadj < (double) .4999999 || aadj > (double) .5000001)
                        break;
                } else if (aadj < (double) .4999999 / FLT_RADIX)
                    break;
            }
        cont:
        Bfree(bb);
        Bfree(bd);
        Bfree(bs);
        Bfree(delta);
    }
#ifdef Avoid_Underflow
    if (scale) {
        dword0(rv0) = Exp_1 - 2 * _P * Exp_msk1;
        dword1(rv0) = 0;
        dval(rv) *= dval(rv0);
#ifndef NO_ERRNO
        /* try to avoid the bug of testing an 8087 register value */
        if ((dword0(rv) & Exp_mask) == 0)
            __set_errno(ERANGE);
#endif
    }
#endif /* Avoid_Underflow */
    retfree:
    Bfree(bb);
    Bfree(bd);
    Bfree(bs);
    Bfree(bd0);
    Bfree(delta);
    ret:
    if (se)
        *se = (char *) s;
    return sign ? -dval(rv) : dval(rv);
}

double
strtod(const char *__restrict s00, char **__restrict se) {
    struct _clib2 *__clib2 = __CLIB2;
    return strtod_l(s00, se, __clib2->_current_locale);
}

#if defined(_HAVE_LONG_DOUBLE) && defined(_LDBL_EQ_DBL)
#ifdef _HAVE_ALIAS_ATTRIBUTE
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wattribute-alias="
extern long double strtold(const char *, char **) __attribute__ ((__alias__ ("strtod")));
#else
long double
strtold (const char * nptr, char ** endptr) {
    return (long double) strtod(nptr, endptr);
}
#endif
#endif

/*
 * These two functions are not quite correct as they return true for
 * zero, however they are 'good enough' for the test in strtof below
 * as we only need to know whether the double test is false when
 * the float test is true.
 */
static inline int
isdenorm(double d) {
    U u;
    dval(u) = d;
    return (dword0(u) & Exp_mask) == 0;
}

static inline int
isdenormf(float f) {
    union {
        float f;
        uint32_t i;
    } u;
    u.f = f;
    return (u.i & 0x7f800000) == 0;
}

float
strtof_l(const char *__restrict s00, char **__restrict se, locale_t loc) {
    double val = strtod_l(s00, se, loc);
    if (isnan(val))
        return signbit(val) ? -nanf("") : nanf("");
    float retval = (float) val;
#ifndef NO_ERRNO
    if ((isinf(retval) && !isinf(val)) || (isdenormf(retval) && !isdenorm(val)))
        __set_errno(ERANGE);
#endif
    return retval;
}

float
strtof(const char *__restrict s00, char **__restrict se) {
    struct _clib2 *__clib2 = __CLIB2;
    double val = strtod_l(s00, se, __clib2->_current_locale);
    if (isnan(val))
        return signbit(val) ? -nanf("") : nanf("");
    float retval = (float) val;
#ifndef NO_ERRNO
    if ((isinf(retval) && !isinf(val)) || (isdenormf(retval) && !isdenorm(val)))
        __set_errno(ERANGE);
#endif
    return retval;
}
