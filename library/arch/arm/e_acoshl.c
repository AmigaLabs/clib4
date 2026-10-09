/*
 * e_acoshl.c - acoshl for ARM.
 *
 * On this toolchain long double is the 64-bit double, on PowerPC and on ARM
 * alike (__SIZEOF_LONG_DOUBLE__ is 8). The 128-bit word algorithm in
 * math/e_acoshl.c reads a 16-byte shape that does not exist, so on ARM the
 * double algorithm is the one that is correct. The PowerPC file is unchanged.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <math.h>

long double
acoshl(long double x) {
    return acosh((double) x);
}
