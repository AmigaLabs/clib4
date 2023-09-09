/*
 * $Id: math_invtrig.h,v 1.0 2022-03-10 12:04:23 clib2devs Exp $
*/

#define	BIAS		(LDBL_MAX_EXP - 1)
#define	MANH_SIZE	LDBL_MANH_SIZE

/* Approximation thresholds. */
#define	ASIN_LINEAR	(BIAS - 32)	/* 2**-32 */
#define	ACOS_CONST	(BIAS - 65)	/* 2**-65 */
#define	ATAN_CONST	(BIAS + 65)	/* 2**65 */
#define	ATAN_LINEAR	(BIAS - 32)	/* 2**-32 */

/* 0.95 */
#define	THRESH	((0xe666666666666666ULL>>(64-(MANH_SIZE-1)))|LDBL_NBIT)

/* Constants shared by the long double inverse trig functions. */
#define	pS0	_ItL_pS0
#define	pS1	_ItL_pS1
#define	pS2	_ItL_pS2
#define	pS3	_ItL_pS3
#define	pS4	_ItL_pS4
#define	pS5	_ItL_pS5
#define	pS6	_ItL_pS6
#define	qS1	_ItL_qS1
#define	qS2	_ItL_qS2
#define	qS3	_ItL_qS3
#define	qS4	_ItL_qS4
#define	qS5	_ItL_qS5
#define	atanhi	_ItL_atanhi
#define	atanlo	_ItL_atanlo
#define	aT	_ItL_aT
#define	pi_lo	_ItL_pi_lo

#define	pio2_hi	atanhi[3]
#define	pio2_lo	atanlo[3]
#define	pio4_hi	atanhi[1]

#ifdef STRUCT_DECLS
typedef struct longdouble {
	uint64_t mant;
	uint16_t expsign;
} LONGDOUBLE;
#else
typedef long double LONGDOUBLE;
#endif

extern const LONGDOUBLE pS0, pS1, pS2, pS3, pS4, pS5, pS6;
extern const LONGDOUBLE qS1, qS2, qS3, qS4, qS5;
extern const LONGDOUBLE atanhi[], atanlo[], aT[];
extern const LONGDOUBLE pi_lo;

#ifndef STRUCT_DECLS

static inline long double
P(long double x)
{

    return (x * (pS0 + x * (pS1 + x * (pS2 + x * (pS3 + x * \
		(pS4 + x * (pS5 + x * pS6)))))));
}

static inline long double
Q(long double x)
{

    return (1.0 + x * (qS1 + x * (qS2 + x * (qS3 + x * (qS4 + x * qS5)))));
}

static inline long double
T_even(long double x)
{

    return (aT[0] + x * (aT[2] + x * (aT[4] + x * (aT[6] + x * \
		(aT[8] + x * (aT[10] + x * aT[12]))))));
}

static inline long double
T_odd(long double x)
{

    return (aT[1] + x * (aT[3] + x * (aT[5] + x * (aT[7] + x * \
		(aT[9] + x * aT[11])))));
}

#endif