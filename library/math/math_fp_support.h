/*
 * $Id: math_fp_support.h,v 1.5 2006-01-08 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_FP_SUPPORT_H
#define _MATH_FP_SUPPORT_H

#include <endian.h>
#include <stdint.h>

union IEEEl2bits {
    long double	e;
    struct {
        unsigned int	sign	:1;
        unsigned int	exp	    :11;
        unsigned int	manh	:20;
        unsigned int	manl	:32;
    } bits;
    struct {
        unsigned int	manh	:20;
        unsigned int	manl	:32;
        unsigned int	expsign	:12;
    } xbits;
};

union IEEEf2bits {
    float	f;
    struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        unsigned int	man	 :23;
        unsigned int	exp	 :8;
        unsigned int	sign :1;
#else /* _BIG_ENDIAN */
        unsigned int	sign :1;
		unsigned int	exp	 :8;
		unsigned int	man	 :23;
#endif
    } bits;
};

struct Double {
    double	a;
    double	b;
};

union IEEEd2bits {
    double	d;
    struct {
#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
        uint32_t manl : 32;
        uint32_t manh : 20;
        uint32_t exp : 11;
        uint32_t sign : 1;
#else // Big endian
        uint32_t sign : 1;
        uint32_t exp : 11;
        uint32_t manh : 20;
        uint32_t manl : 32;
#endif
    } bits;
};

#define	mask_nbit_l(u)	((void)0)
#define	LDBL_IMPLICIT_NBIT
#define	LDBL_NBIT	    0

#define	LDBL_MANH_SIZE	20
#define	LDBL_MANL_SIZE	32

/* 'Portable' raw representations of three IEEE floating point formats. */
union ieee_double {
    double			value;
    unsigned long	raw[2];
};

union ieee_single {
    float			value;
    unsigned long	raw[1];
};

#endif /* _MATH_FP_SUPPORT_H */
