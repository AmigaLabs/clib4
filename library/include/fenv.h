/*
 * $Id: fenv.h,v 1.5 2022-03-09 19:09:00 clib2devs Exp $
*/

#ifndef _FENV_H
#define _FENV_H

#include <stdint.h>
#include <features.h>
#include <endian.h>

typedef uint32_t fenv_t;
typedef uint32_t fexcept_t;

/* Exception flags */
#define    FE_INEXACT      0x02000000
#define    FE_DIVBYZERO    0x04000000
#define    FE_UNDERFLOW    0x08000000
#define    FE_OVERFLOW     0x10000000
#define    FE_INVALID      0x20000000    /* all types of invalid FP ops */

/*
 * The PowerPC architecture has extra invalid flags that indicate the
 * specific type of invalid operation occurred.  These flags may be
 * tested, set, and cleared---but not masked---separately.  All of
 * these bits are cleared when FE_INVALID is cleared, but only
 * FE_VXSOFT is set when FE_INVALID is explicitly set in software.
 */
#define    FE_VXCVI    0x00000100    /* invalid integer convert */
#define    FE_VXSQRT   0x00000200    /* square root of a negative */
#define    FE_VXSOFT   0x00000400    /* software-requested exception */
#define    FE_VXVC     0x00080000    /* ordered comparison involving NaN */
#define    FE_VXIMZ    0x00100000    /* inf * 0 */
#define    FE_VXZDZ    0x00200000    /* 0 / 0 */
#define    FE_VXIDI    0x00400000    /* inf / inf */
#define    FE_VXISI    0x00800000    /* inf - inf */
#define    FE_VXSNAN   0x01000000    /* operation on a signalling NaN */
#define    FE_ALL_INVALID    (FE_VXCVI | FE_VXSQRT | FE_VXSOFT | FE_VXVC | \
             FE_VXIMZ | FE_VXZDZ | FE_VXIDI | FE_VXISI | \
             FE_VXSNAN | FE_INVALID)
#define    FE_ALL_EXCEPT    (FE_DIVBYZERO | FE_INEXACT | \
             FE_ALL_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define    FE_TONEAREST    0x0000
#define    FE_TOWARDZERO   0x0001
#define    FE_UPWARD       0x0002
#define    FE_DOWNWARD     0x0003
#define    _ROUND_MASK    (FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO)

__BEGIN_DECLS

/* Default floating-point environment */
extern const fenv_t __fe_dfl_env;
#define FE_DFL_ENV (&__fe_dfl_env)

/* We need to be able to map status flag positions to mask flag positions */
#define _FPUSW_SHIFT    22
#define _ENABLE_MASK    ((FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW) >> _FPUSW_SHIFT)

#ifdef __SPE__
    #define	__mffs(__env) __asm __volatile("mfspr %0, 512" : "=r" (*(__env)))
    #define	__mtfsf(__env) __asm __volatile("mtspr 512, %0; isync" :: "r" ((__env)))
#else
    /* Equivalent to fegetenv, but returns an unsigned int instead of taking a pointer.  */
    #define fegetenv_register() \
      ({ unsigned int fscr; asm volatile ("mfspefscr %0" : "=r" (fscr)); fscr; })

    /* Equivalent to fesetenv, but takes an unsigned int instead of a pointer.  */
    #define fesetenv_register(fscr) \
      ({ asm volatile ("mtspefscr %0" : : "r" (fscr)); })
#endif

union __fpscr {
    double __d;
    struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        fenv_t __reg;
        uint32_t __junk;
#else
        uint32_t __junk;
        fenv_t __reg;
#endif
    } __bits;
};

extern int feclearexcept(int __excepts);
extern int fegetexceptflag(fexcept_t *__flagp, int __excepts);
extern int fesetexceptflag(const fexcept_t *__flagp, int __excepts);
extern int feraiseexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int fegetround(void);
extern int fesetround(int __round);
extern int fegetenv(fenv_t *__envp);
extern int feholdexcept(fenv_t *__envp);
extern int fesetenv(const fenv_t *__envp);
extern int feupdateenv(const fenv_t *__envp);
extern int fegetexcept(void);
extern int feenableexcept(int __mask);
extern int fedisableexcept(int __mask);

__END_DECLS

#endif /* _FENV_H */
