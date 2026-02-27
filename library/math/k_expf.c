/*
 * $Id: math_k_expf.c,v 1.0 2023-07-19 12:04:23 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static const uint32_t k = 235;            /* constant for reduction */
static const float kln2 = 162.88958740F;    /* k * ln2 */

/*
 * See k_exp.c for details.
 *
 * Input:  ln(FLT_MAX) <= x < ln(2 * FLT_MAX / FLT_MIN_DENORM) ~= 192.7
 * Output: 2**127 <= y < 2**128
 */
static float
__frexp_expf(float x, int *expt) {
    double exp_x;
    uint32_t hx;

    exp_x = expf(x - kln2);
    GET_FLOAT_WORD(hx, exp_x);
    *expt = (hx >> 23) - (0x7f + 127) + k;
    SET_FLOAT_WORD(exp_x, (hx & 0x7fffff) | ((0x7f + 127) << 23));
    return (exp_x);
}

float
__ldexp_expf(float x, int expt) {
    float exp_x, scale;
    int ex_expt;

    exp_x = __frexp_expf(x, &ex_expt);
    expt += ex_expt;
    SET_FLOAT_WORD(scale, (0x7f + expt) << 23);
    return (exp_x * scale);
}

float complex
__ldexp_cexpf(float complex z, int expt) {
    float x, y, exp_x, scale1, scale2;
    int ex_expt, half_expt;

    x = crealf(z);
    y = cimagf(z);
    exp_x = __frexp_expf(x, &ex_expt);
    expt += ex_expt;

    half_expt = expt / 2;
    SET_FLOAT_WORD(scale1, (0x7f + half_expt) << 23);
    half_expt = expt - half_expt;
    SET_FLOAT_WORD(scale2, (0x7f + half_expt) << 23);

    return (CMPLXF(cosf(y) * exp_x * scale1 * scale2,
                   sinf(y) * exp_x * scale1 * scale2));
}
