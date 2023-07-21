/*
 * $Id: math_fp_support.h,v 1.5 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_FP_SUPPORT_H
#define _MATH_FP_SUPPORT_H

#include <endian.h>

/* 'Portable' raw representations of three IEEE floating point formats. */
union ieee_long_double
{
	long double		value;
	unsigned long	raw[3];
};

union ieee_double
{
	double			value;
	unsigned long	raw[2];
};

union ieee_single
{
	float			value;
	unsigned long	raw[1];
};

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
        unsigned int	sign	:1;
		unsigned int	exp	    :8;
		unsigned int	man	    :23;
    } bits;
};

struct Double {
    double	a;
    double	b;
};

#define	DBL_MANH_SIZE	20
#define	DBL_MANL_SIZE	32

union IEEEd2bits {
    double	d;
    struct {
        unsigned int	sign	:1;
		unsigned int	exp 	:11;
		unsigned int	manh	:20;
		unsigned int	manl	:32;
    } bits;
};

#define	mask_nbit_l(u)	((void)0)
#define	LDBL_IMPLICIT_NBIT
#define	LDBL_NBIT	    0

#define	LDBL_MANH_SIZE	20
#define	LDBL_MANL_SIZE	32

#define	LDBL_TO_ARRAY32(u, a) do {			\
	(a)[0] = (uint32_t)(u).bits.manl;		\
	(a)[1] = (uint32_t)(u).bits.manh;		\
} while(0)

/****************************************************************************/

#endif /* _MATH_FP_SUPPORT_H */
