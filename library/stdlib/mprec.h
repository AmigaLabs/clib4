#ifndef __MPREC_H__
#define __MPREC_H__

#define hexdig  __hexdig

struct _Bigint {
    int _k, _maxwds, _sign, _wds;
    uint32_t _x[1];
};
typedef struct _Bigint _Bigint;

union double_union {
    double d;
    uint32_t i[2];
};

#define ULbits 32
#define kshift 5
#define kmask 31
#define ALL_ON 0xffffffff

#define Scale_Bit            0x10
#define n_bigtens            5

#define Exp_shift            20
#define Exp_shift1           20
#define Exp_msk1             ((uint32_t) 0x100000L)
#define Exp_msk11            ((uint32_t) 0x100000L)
#define Exp_mask             ((uint32_t) 0x7ff00000L)
#define Exp_1                ((uint32_t) 0x3ff00000L)
#define Tiny0                0
#define Tiny1                1
#define _P                   53
#define Bias                 1023
#define Emin                 (-1022)
#define Ebits                11
#define Bndry_mask           ((uint32_t) 0xfffffL)
#define Bndry_mask1          ((uint32_t) 0xfffffL)
#define Frac_mask            ((uint32_t) 0xfffffL)
#define Frac_mask1           ((uint32_t) 0xfffffL)
#define Ten_pmax             22
#define Flt_Rounds           1 // Check
#define Log2P                1
#define LSB                  1
#define Big0                 (Frac_mask1 | Exp_msk1 * (DBL_MAX_EXP + Bias - 1))
#define Big1                 ((uint32_t) 0xffffffffL)

#define word0(x) (x.i[0])
#define word1(x) (x.i[1])
#define dword0(x) word0(x)
#define dword1(x) word1(x)
#define dval(x) (x.d)

#define __get_hexdig(x) __hexdig_fun(x)

#define Storeinc(a,b,c) (*(a)++ = ((b) << 16) | ((c) & 0xffff))

#ifdef Unsigned_Shifts
#define Sign_Extend(a,b) if (b < 0) a |= (uint32_t) 0xffff0000;
#else
#define Sign_Extend(a,b) /*no-op*/
#endif

struct FPI;
extern double ulp(double x);
extern double b2d(_Bigint *a, int *e);
extern _Bigint *Balloc(int k);
extern void Bfree(_Bigint *v);
extern _Bigint *multadd(_Bigint *, int, int);
extern _Bigint *s2b(const char *, int, int, uint32_t);
extern _Bigint *i2b(int);
extern _Bigint *mult(_Bigint *, _Bigint *);
extern _Bigint *pow5mult(_Bigint *, int k);
extern int hi0bits(uint32_t);
extern int lo0bits(uint32_t *);
extern _Bigint *d2b(double d, int *e, int *bits);
extern _Bigint *lshift(_Bigint *b, int k);
//int match(const char **, char *);
extern _Bigint *diff(_Bigint *a, _Bigint *b);
extern int cmp(_Bigint *a, _Bigint *b);
extern int gethex(const char **sp, const struct FPI *fpi, int32_t *exp, _Bigint **bp, int sign, locale_t loc);
extern double ratio(_Bigint *a, _Bigint *b);
extern uint32_t any_on(_Bigint *b, int k);
extern void copybits(uint32_t *c, int n, _Bigint *b);
extern void hexdig_init(void);
extern unsigned char __hexdig_fun (unsigned char);

extern double _strtod_l(const char *__restrict s00, char **__restrict se, locale_t loc);
extern int _strtorx_l(const char *, char **, int, void *, locale_t);
extern int _strtodg_l(const char *s00, char **se, struct FPI *fpi, int32_t *exp, uint32_t *bits, locale_t);
extern int	hexnan (const char **sp, const struct FPI *fpi, uint32_t *x0);

#define Bcopy(x, y) memcpy((char *)&x->_sign, (char *)&y->_sign, y->_wds*sizeof(int32_t) + 2*sizeof(int))

#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b

extern const __float64 tinytens[];
extern const __float64 bigtens[];
extern const double tens[];
extern unsigned char hexdig[];

#endif