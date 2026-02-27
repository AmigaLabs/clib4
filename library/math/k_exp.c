/*
 * $Id: math_k_exp.c,v 1.0 2023-07-19 12:04:23 clib4devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const uint32_t k = 1799;		/* constant for reduction */
static const double kln2 =  1246.97177782734161156;	/* k * ln2 */

static double
__frexp_exp(double x, int *expt) {
    double exp_x;
    uint32_t hx;

    /*
     * We use exp(x) = exp(x - kln2) * 2**k, carefully chosen to
     * minimize |exp(kln2) - 2**k|.  We also scale the exponent of
     * exp_x to MAX_EXP so that the result can be multiplied by
     * a tiny number without losing accuracy due to denormalization.
     */
    exp_x = exp(x - kln2);
    GET_HIGH_WORD(hx, exp_x);
    *expt = (hx >> 20) - (0x3ff + 1023) + k;
    SET_HIGH_WORD(exp_x, (hx & 0xfffff) | ((0x3ff + 1023) << 20));
    return (exp_x);
}

/*
 * __ldexp_exp(x, expt) and __ldexp_cexp(x, expt) compute exp(x) * 2**expt.
 * They are intended for large arguments (real part >= ln(DBL_MAX))
 * where care is needed to avoid overflow.
 *
 * The present implementation is narrowly tailored for our hyperbolic and
 * exponential functions.  We assume expt is small (0 or -1), and the caller
 * has filtered out very large x, for which overflow would be inevitable.
 */

double
__ldexp_exp(double x, int expt) {
    double exp_x, scale;
    int ex_expt;

    exp_x = __frexp_exp(x, &ex_expt);
    expt += ex_expt;
    INSERT_WORDS(scale, (0x3ff + expt) << 20, 0);
    return (exp_x * scale);
}

double complex
__ldexp_cexp(double complex z, int expt) {
    double x, y, exp_x, scale1, scale2;
    int ex_expt, half_expt;

    x = creal(z);
    y = cimag(z);
    exp_x = __frexp_exp(x, &ex_expt);
    expt += ex_expt;

    /*
     * Arrange so that scale1 * scale2 == 2**expt.  We use this to
     * compensate for scalbn being horrendously slow.
     */
    half_expt = expt / 2;
    INSERT_WORDS(scale1, (0x3ff + half_expt) << 20, 0);
    half_expt = expt - half_expt;
    INSERT_WORDS(scale2, (0x3ff + half_expt) << 20, 0);

    return (CMPLX(cos(y) * exp_x * scale1 * scale2,
                  sin(y) * exp_x * scale1 * scale2));
}