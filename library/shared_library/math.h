#ifndef _MATH_H__
#define _MATH_H__

#include <stdint.h>

typedef int si_int;
typedef unsigned su_int;
typedef long long di_int;
typedef unsigned long long du_int;
typedef union {
    di_int all;
    struct {
        si_int high;
        su_int low;
    } s;
} dwords;

typedef union {
    du_int all;
    struct {
        su_int high;
        su_int low;
    } s;
} udwords;

typedef union {
    su_int u;
    float f;
} float_bits;

typedef union {
    udwords u;
    double f;
} double_bits;

typedef union {
    long double ld;
    struct {
        double hi;
        double lo;
    }s;
} DD;

typedef union {
    int64_t x;
    double d;
} doublebits;

typedef struct {
    udwords high;
    udwords low;
} uqwords;

typedef union {
    uqwords u;
    long double f;
} long_double_bits;

di_int __fixsfdi(float a);
di_int __fixdfdi(double a);
du_int __udivmoddi4(du_int a, du_int b, du_int *rem);
du_int __umoddi3(du_int a, du_int b);
float _Complex __mulsc3(float __a, float __b, float __c, float __d);
double _Complex __muldc3(double __a, double __b, double __c, double __d);
du_int __udivdi3(du_int a, du_int b);
di_int __moddi3(di_int a, di_int b);
di_int __divdi3(di_int a, di_int b);
double __floatdidf(di_int a);
di_int __unorddf2 (double a, double b);
long double __floatunditf(uint64_t a);
double __floatundidf(du_int a);

#endif /* _MATH_H__ */