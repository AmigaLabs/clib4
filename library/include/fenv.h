/*
 * $Id: fenv.h,v 1.5 2022-03-09 19:09:00 clib4devs Exp $
*/

#ifndef _FENV_H
#define _FENV_H

#include <stdint.h>
#include <features.h>
#include <endian.h>
#include <stdbool.h>

#ifndef	__fenv_static
#define	__fenv_static	static
#endif


#if defined __NO_FPRS__ && !defined _SOFT_FLOAT /* E500 */

    /* Define bits representing the exception.  We use the bit positions of
       the appropriate bits in the SPEFSCR...  */
    enum
      {
        FE_INEXACT = 1 << (63 - 42),
    #define FE_INEXACT	FE_INEXACT
        FE_INVALID = 1 << (63 - 43),
    #define FE_INVALID	FE_INVALID
        FE_DIVBYZERO = 1 << (63 - 44),
    #define FE_DIVBYZERO	FE_DIVBYZERO
        FE_UNDERFLOW = 1 << (63 - 45),
    #define FE_UNDERFLOW	FE_UNDERFLOW
        FE_OVERFLOW = 1 << (63 - 46)
    #define FE_OVERFLOW	FE_OVERFLOW
      };

    #define FE_ALL_EXCEPT \
        (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

    #define	FE_ALL_INVALID	FE_INVALID

    /* The E500 support all of the four defined rounding modes.  We use
       the bit pattern in the SPEFSCR as the values for the appropriate
       macros.  */
    enum
      {
        FE_TONEAREST = 0,
    #define FE_TONEAREST	FE_TONEAREST
        FE_TOWARDZERO = 1,
    #define FE_TOWARDZERO	FE_TOWARDZERO
        FE_UPWARD = 2,
    #define FE_UPWARD	FE_UPWARD
        FE_DOWNWARD = 3
    #define FE_DOWNWARD	FE_DOWNWARD
      };

    /* Type representing exception flags.  */
    typedef unsigned int fexcept_t;
    typedef uint32_t fenv_t;
#else
    /* Define bits representing the exception.  We use the bit positions of
       the appropriate bits in the FPSCR...  */
    enum
    {
        FE_INEXACT = 1 << (31 - 6),
    #define FE_INEXACT	FE_INEXACT
        FE_DIVBYZERO = 1 << (31 - 5),
    #define FE_DIVBYZERO	FE_DIVBYZERO
        FE_UNDERFLOW = 1 << (31 - 4),
    #define FE_UNDERFLOW	FE_UNDERFLOW
        FE_OVERFLOW = 1 << (31 - 3),
    #define FE_OVERFLOW	FE_OVERFLOW

        /* ... except for FE_INVALID, for which we use bit 31. FE_INVALID
           actually corresponds to bits 7 through 12 and 21 through 23
           in the FPSCR, but we can't use that because the current draft
           says that it must be a power of 2.  Instead we use bit 2 which
           is the summary bit for all the FE_INVALID exceptions, which
           kind of makes sense.  */
        FE_INVALID = 1 << (31 - 2),
    #define FE_INVALID	FE_INVALID

    #ifdef __USE_GNU
        /* Breakdown of the FE_INVALID bits. Setting FE_INVALID on an
           input to a routine is equivalent to setting all of these bits;
           FE_INVALID will be set on output from a routine iff one of
           these bits is set.  Note, though, that you can't disable or
           enable these exceptions individually.  */

        /* Operation with SNaN. */
            FE_INVALID_SNAN = 1 << (31 - 7),
        # define FE_INVALID_SNAN	FE_INVALID_SNAN

            /* Inf - Inf */
            FE_INVALID_ISI = 1 << (31 - 8),
        # define FE_INVALID_ISI		FE_INVALID_ISI

            /* Inf / Inf */
            FE_INVALID_IDI = 1 << (31 - 9),
        # define FE_INVALID_IDI		FE_INVALID_IDI

            /* 0 / 0 */
            FE_INVALID_ZDZ = 1 << (31 - 10),
        # define FE_INVALID_ZDZ		FE_INVALID_ZDZ

            /* Inf * 0 */
            FE_INVALID_IMZ = 1 << (31 - 11),
        # define FE_INVALID_IMZ		FE_INVALID_IMZ

            /* Comparison with NaN or SNaN.  */
            FE_INVALID_COMPARE = 1 << (31 - 12),
        # define FE_INVALID_COMPARE	FE_INVALID_COMPARE

            /* Invalid operation flag for software (not set by hardware).  */
            /* Note that some chips don't have this implemented, presumably
               because no-one expected anyone to write software for them %-).  */
            FE_INVALID_SOFTWARE = 1 << (31 - 21),
        # define FE_INVALID_SOFTWARE	FE_INVALID_SOFTWARE

            /* Square root of negative number (including -Inf).  */
            /* Note that some chips don't have this implemented.  */
            FE_INVALID_SQRT = 1 << (31 - 22),
        # define FE_INVALID_SQRT	FE_INVALID_SQRT

            /* Conversion-to-integer of a NaN or a number too large or too small.  */
            FE_INVALID_INTEGER_CONVERSION = 1 << (31 - 23)
        # define FE_INVALID_INTEGER_CONVERSION	FE_INVALID_INTEGER_CONVERSION

        # define FE_ALL_INVALID \
                (FE_INVALID_SNAN | FE_INVALID_ISI | FE_INVALID_IDI | FE_INVALID_ZDZ \
             | FE_INVALID_IMZ | FE_INVALID_COMPARE | FE_INVALID_SOFTWARE \
             | FE_INVALID_SQRT | FE_INVALID_INTEGER_CONVERSION)
    #endif /* !__USE_GNU */
    };

    #define FE_ALL_EXCEPT \
        (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

    /* PowerPC chips support all of the four defined rounding modes.  We
       use the bit pattern in the FPSCR as the values for the
       appropriate macros.  */
    enum
    {
        FE_TONEAREST =  0,
    #define FE_TONEAREST	FE_TONEAREST
        FE_TOWARDZERO = 1,
    #define FE_TOWARDZERO	FE_TOWARDZERO
        FE_UPWARD =     2,
    #define FE_UPWARD	    FE_UPWARD
        FE_DOWNWARD =   3
    #define FE_DOWNWARD	    FE_DOWNWARD
    };

    /* Type representing exception flags.  */
    typedef unsigned int fexcept_t;

    /* Type representing floating-point environment.  We leave it as 'double'
       for efficiency reasons (rather than writing it to a 32-bit integer). */
    typedef uint32_t fenv_t;
#endif

#define _ROUND_MASK (FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO)

__BEGIN_DECLS

/* Default floating-point environment */
extern const fenv_t __fe_dfl_env;
#define FE_DFL_ENV (&__fe_dfl_env)

/* We need to be able to map status flag positions to mask flag positions */
#define _FPUSW_SHIFT    22
#define _ENABLE_MASK    ((FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW) >> _FPUSW_SHIFT)

#ifndef _SOFT_FLOAT
    #ifdef __SPE__
        #define	__mffs(__env) __asm __volatile("mfspr %0, 512" : "=r" ((__env)->__bits.__reg))
        #define	__mtfsf(__env) __asm __volatile("mtspr 512,%0;isync" :: "r" ((__env).__bits.__reg))
    #else
        #define	__mffs(__env) __asm __volatile("mffs %0" : "=f" ((__env)->__d))
        #define	__mtfsf(__env) __asm __volatile("mtfsf 255,%0" :: "f" ((__env).__d))
    #endif
#else
    #define	__mffs(__env)
    #define	__mtfsf(__env)
#endif

union __fpscr {
    double __d;
    struct {
#if __BYTE_ORDER__ == LITTLE_ENDIAN
        fenv_t __reg;
        uint32_t __junk;
#else
        uint32_t __junk;
		fenv_t __reg;
#endif
    } __bits;
};

struct rm_ctx {
    fenv_t env;
    bool updated_status;
};

__fenv_static inline int
feclearexcept(int __excepts)
{
    union __fpscr __r;

#ifdef __USE_GNU
    if (__excepts & FE_INVALID)
        __excepts |= FE_ALL_INVALID;
#endif
    __mffs(&__r);
    __r.__bits.__reg &= ~__excepts;
    __mtfsf(__r);
    return (0);
}

__fenv_static inline int
fegetexceptflag(fexcept_t *__flagp, int __excepts)
{
    union __fpscr __r;

    __mffs(&__r);
    *__flagp = __r.__bits.__reg & __excepts;
    return (0);
}

__fenv_static inline int
fesetexceptflag(const fexcept_t *__flagp, int __excepts)
{
    union __fpscr __r;

#ifdef __USE_GNU
    if (__excepts & FE_INVALID)
        __excepts |= FE_ALL_INVALID;
#endif
    __mffs(&__r);
    __r.__bits.__reg &= ~__excepts;
    __r.__bits.__reg |= *__flagp & __excepts;
    __mtfsf(__r);
    return (0);
}

#ifdef __SPE__
extern int	feraiseexcept(int __excepts);
#else
__fenv_static inline int
feraiseexcept(int __excepts)
{
    union __fpscr __r;

#ifdef __USE_GNU
    if (__excepts & FE_INVALID)
        __excepts |= FE_INVALID_SOFTWARE;
#endif
    __mffs(&__r);
    __r.__bits.__reg |= __excepts;
    __mtfsf(__r);
    return (0);
}
#endif

__fenv_static inline int
fetestexcept(int __excepts)
{
    union __fpscr __r;

    __mffs(&__r);
    return (__r.__bits.__reg & __excepts);
}

__fenv_static inline int
fegetround(void)
{
    union __fpscr __r;

    __mffs(&__r);
    return (__r.__bits.__reg & _ROUND_MASK);
}

__fenv_static inline int
fesetround(int __round)
{
    union __fpscr __r;

    if (__round & ~_ROUND_MASK)
        return (-1);
    __mffs(&__r);
    __r.__bits.__reg &= ~_ROUND_MASK;
    __r.__bits.__reg |= __round;
    __mtfsf(__r);
    return (0);
}

__fenv_static inline int
fegetenv(fenv_t *__envp)
{
    union __fpscr __r;

    __mffs(&__r);
    *__envp = __r.__bits.__reg;
    return (0);
}

__fenv_static inline int
feholdexcept(fenv_t *__envp)
{
    union __fpscr __r;

    __mffs(&__r);
    *__envp = __r.__bits.__reg;
    __r.__bits.__reg &= ~(FE_ALL_EXCEPT | _ENABLE_MASK);
    __mtfsf(__r);
    return (0);
}

__fenv_static inline int
fesetenv(const fenv_t *__envp)
{
    union __fpscr __r;

    __r.__bits.__reg = *__envp;
    __mtfsf(__r);
    return (0);
}

__fenv_static inline int
feupdateenv(const fenv_t *__envp)
{
    union __fpscr __r;

    __mffs(&__r);
    __r.__bits.__reg &= FE_ALL_EXCEPT;
    __r.__bits.__reg |= *__envp;
    __mtfsf(__r);
    return (0);
}

__fenv_static inline int
feenableexcept(int __mask)
{
	union __fpscr __r;
	fenv_t __oldmask;

	__mffs(&__r);
	__oldmask = __r.__bits.__reg;
	__r.__bits.__reg |= (__mask & FE_ALL_EXCEPT) >> _FPUSW_SHIFT;
	__mtfsf(__r);
	return ((__oldmask & _ENABLE_MASK) << _FPUSW_SHIFT);
}

__fenv_static inline int
fedisableexcept(int __mask)
{
	union __fpscr __r;
	fenv_t __oldmask;

	__mffs(&__r);
	__oldmask = __r.__bits.__reg;
	__r.__bits.__reg &= ~((__mask & FE_ALL_EXCEPT) >> _FPUSW_SHIFT);
	__mtfsf(__r);
	return ((__oldmask & _ENABLE_MASK) << _FPUSW_SHIFT);
}

/* We currently provide no external definition of fegetexcept(). */
static inline int
fegetexcept(void)
{
	union __fpscr __r;

	__mffs(&__r);
	return ((__r.__bits.__reg & _ENABLE_MASK) << _FPUSW_SHIFT);
}

__END_DECLS

#endif /* _FENV_H */
