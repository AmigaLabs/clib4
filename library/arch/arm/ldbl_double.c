/*
 * ldbl_double.c - long double math for ARM.
 *
 * On this toolchain long double is the 64-bit double (float.h: LDBL_MANT_DIG
 * is DBL_MANT_DIG, _LDBL_EQ_DBL is 1). The PowerPC files for these functions
 * (math/e_acosl.c, e_asinl.c, e_atan2l.c, s_atanl.c, s_copysignl.c and
 * s_roundevenl.c) read a 128-bit shape with the x87 or double-double word
 * layout, which an ARM long double does not have. On ARM they forward to the
 * double functions, which are correct. The PowerPC files are unchanged.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <math.h>

long double
acosl(long double x) {
    return acos((double) x);
}

long double
asinl(long double x) {
    return asin((double) x);
}

long double
atanl(long double x) {
    return atan((double) x);
}

long double
atan2l(long double y, long double x) {
    return atan2((double) y, (double) x);
}

long double
copysignl(long double x, long double y) {
    return copysign((double) x, (double) y);
}

long double
roundevenl(long double x) {
    return roundeven((double) x);
}
