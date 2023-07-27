/*
 * $Id: fenv.c,v 1.1 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <fenv.h>

#ifdef __GNUC_GNU_INLINE__
#error "This file must be compiled with C99 'inline' semantics"
#endif

#ifdef __SPE__
#include <sys/spr.h>

typedef union {
    fenv_t fenv;
    unsigned int l[2];
} fenv_union_t;

# define ASM_INPUT_0 "0" (r0)
# define ASM_INPUT_1 ASM_INPUT_0, "1" (r3)
# define ASM_INPUT_2 ASM_INPUT_1, "2" (r4)
# define ASM_INPUT_3 ASM_INPUT_2, "3" (r5)
# define ASM_INPUT_4 ASM_INPUT_3, "4" (r6)
# define ASM_INPUT_5 ASM_INPUT_4, "5" (r7)
# define ASM_INPUT_6 ASM_INPUT_5, "6" (r8)

# define INTERNAL_SYSCALL_DECL(err) long int err __attribute__ ((unused))
# define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({									\
    register long int r0  __asm__ ("r0");				\
    register long int r3  __asm__ ("r3");				\
    register long int r4  __asm__ ("r4");				\
    register long int r5  __asm__ ("r5");				\
    register long int r6  __asm__ ("r6");				\
    register long int r7  __asm__ ("r7");				\
    register long int r8  __asm__ ("r8");				\
    register long int r9  __asm__ ("r9");				\
    register long int r10 __asm__ ("r10");				\
    register long int r11 __asm__ ("r11");				\
    register long int r12 __asm__ ("r12");				\
    LOADARGS_##nr(name, args);						\
    __asm__ __volatile__						\
      ("sc   \n\t"							\
       "mfcr %0"							\
       : "=&r" (r0),							\
	 "=&r" (r3), "=&r" (r4), "=&r" (r5),  "=&r" (r6),  "=&r" (r7),	\
	 "=&r" (r8), "=&r" (r9), "=&r" (r10), "=&r" (r11), "=&r" (r12)	\
       : ASM_INPUT_##nr							\
       : "cr0", "ctr", "memory");					\
    err = r0;								\
    (int) r3;								\
  })
# define INTERNAL_SYSCALL(name, err, nr, args...) \
  INTERNAL_SYSCALL_NCS (__NR_##name, err, nr, ##args)

# define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((void) (val), __builtin_expect ((err) & (1 << 28), 0))

const fenv_t __fe_dfl_env = SPEFSCR_DFLT;
#define PR_GET_FPEXC        11
#define PR_SET_FPEXC        12
# define PR_FP_EXC_SW_ENABLE        0x80        /* Use FPEXC for FP exception enables */
# define PR_FP_EXC_DIV                0x010000        /* floating point divide by zero */
# define PR_FP_EXC_OVF                0x020000        /* floating point overflow */
# define PR_FP_EXC_UND                0x040000        /* floating point underflow */
# define PR_FP_EXC_RES                0x080000        /* floating point inexact result */
# define PR_FP_EXC_INV                0x100000        /* floating point invalid operation */
# define PR_FP_EXC_DISABLED        0        /* FP exceptions disabled */
# define PR_FP_EXC_NONRECOV        1        /* async non-recoverable exc. mode */
# define PR_FP_EXC_ASYNC        2        /* async recoverable exception mode */
# define PR_FP_EXC_PRECISE        3        /* precise exception mode */
#else
const fenv_t __fe_dfl_env = 0x00000000;
#endif

#ifdef __SPE__
#define	PMAX	0x7f7fffff
#define	PMIN	0x00800000

inline int
__fexcepts_from_prctl (int excepts) {
    int result = 0;

    if (excepts & PR_FP_EXC_OVF)
        result |= FE_OVERFLOW;
    if (excepts & PR_FP_EXC_UND)
        result |= FE_UNDERFLOW;
    if (excepts & PR_FP_EXC_INV)
        result |= FE_INVALID;
    if (excepts & PR_FP_EXC_DIV)
        result |= FE_DIVBYZERO;
    if (excepts & PR_FP_EXC_RES)
        result |= FE_INEXACT;

    return result;
}

inline int
__fexcepts_from_spe (int excepts) {
    int result = 0;

    if (excepts & SPEFSCR_FINXS)
        result |= FE_INEXACT;
    if (excepts & SPEFSCR_FDBZS)
        result |= FE_DIVBYZERO;
    if (excepts & SPEFSCR_FUNFS)
        result |= FE_UNDERFLOW;
    if (excepts & SPEFSCR_FOVFS)
        result |= FE_OVERFLOW;
    if (excepts & SPEFSCR_FINVS)
        result |= FE_INVALID;

    return result;
}

inline int
__fexcepts_to_prctl (int excepts) {
    int result = 0;

    if (excepts & FE_INEXACT)
        result |= PR_FP_EXC_RES;
    if (excepts & FE_DIVBYZERO)
        result |= PR_FP_EXC_DIV;
    if (excepts & FE_UNDERFLOW)
        result |= PR_FP_EXC_UND;
    if (excepts & FE_OVERFLOW)
        result |= PR_FP_EXC_OVF;
    if (excepts & FE_INVALID)
        result |= PR_FP_EXC_INV;

    return result;
}

inline int
__fexcepts_to_spe(int excepts) {
    int result = 0;
    if (excepts & FE_INEXACT)
        result |= SPEFSCR_FINXS;
    if (excepts & FE_DIVBYZERO)
        result |= SPEFSCR_FDBZS;
    if (excepts & FE_UNDERFLOW)
        result |= SPEFSCR_FUNFS;
    if (excepts & FE_OVERFLOW)
        result |= SPEFSCR_FOVFS;
    if (excepts & FE_INVALID)
        result |= SPEFSCR_FINVS;
    return result;
}

int
__feraiseexcept_spe(int excepts) {
	uint32_t f;

    f = fegetenv_register ();
    f |= (excepts & SPEFSCR_ALL_EXCEPT);
    fesetenv_register (f);

	if (excepts & SPEFSCR_FINVS)
		__asm __volatile ("efsdiv %0, %0, %1" :: "r"(0), "r"(0));
	if (excepts & SPEFSCR_FDBZS)
		__asm __volatile ("efsdiv %0, %0, %1" :: "r"(1.0f), "r"(0));
	if (excepts & SPEFSCR_FOVFS)
		__asm __volatile ("efsmul %0, %0, %0" :: "r"(PMIN));
	if (excepts & SPEFSCR_FUNFS)
		__asm __volatile ("efsadd %0, %0, %0" :: "r"(PMAX));
	if (excepts & SPEFSCR_FINXS)
		__asm __volatile ("efssub %0, %0, %1" :: "r"(PMIN), "r"(1.0f));

    return (0);
}
#endif

int
feclearexcept(int __excepts) {
#ifndef __SPE__
    union __fpscr __r;

    if (__excepts & FE_INVALID)
        __excepts |= FE_ALL_INVALID;
    __mffs(&__r.__d);
    __r.__bits.__reg &= ~__excepts;
    __mtfsf(__r.__d);
    return (0);
#else
    unsigned int fpescr;
    int excepts_spe = __fexcepts_to_spe(excepts);

    /* Get the current state.  */
    fpescr = fegetenv_register();

    /* Clear the relevant bits.  */
    fpescr &= ~excepts_spe;

    /* Put the new state in effect.  */
    fesetenv_register(fpescr);

    /* Let the kernel know if the "invalid" or "underflow" bit was
    cleared.  */
    if (excepts & (FE_INVALID | FE_UNDERFLOW))
        __fe_note_change();

    /* Success.  */
    return 0;
#endif
}

int
fegetexceptflag(fexcept_t *__flagp, int __excepts) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    *__flagp = __r.__bits.__reg & __excepts;
    return (0);
#else
    unsigned long fpescr;
    (void) __excepts;

    /* Get the current state.  */
    fpescr = fegetenv_register ();

    *__flagp = fpescr & SPEFSCR_ALL_EXCEPT;

    /* Success.  */
    return 0;
#endif
}

int
fesetexceptflag(const fexcept_t *__flagp, int __excepts) {
#ifndef __SPE__
    union __fpscr __r;

    if (__excepts & FE_INVALID)
        __excepts |= FE_ALL_EXCEPT;
    __mffs(&__r.__d);
    __r.__bits.__reg &= ~__excepts;
    __r.__bits.__reg |= *__flagp & __excepts;
    __mtfsf(__r.__d);
    return (0);
#else
    unsigned long old_spefscr, spefscr;
    fexcept_t flag;

    int excepts_spe = __fexcepts_to_spe (excepts);

    /* Get the current state.  */
    old_spefscr = fegetenv_register ();

    /* Ignore exceptions not listed in 'excepts'.  */
    flag = *flagp & excepts_spe;

    /* Replace the exception status */
    spefscr = (old_spefscr & ~excepts_spe) | flag;

    /* Store the new status word (along with the rest of the environment).  */
    fesetenv_register (spefscr);

    /* If the state of the "invalid" or "underflow" flag has changed, inform the kernel.  */
    if (((spefscr ^ old_spefscr) & (SPEFSCR_FINVS | SPEFSCR_FUNFS)) != 0)
        __fe_note_change ();

    /* Success.  */
    return 0;
#endif
}

#ifndef __SPE__
int
feraiseexcept(int __excepts) {
    union __fpscr __r;

    if (__excepts & FE_INVALID)
        __excepts |= FE_VXSOFT;
    __mffs(&__r.__d);
    __r.__bits.__reg |= __excepts;
    __mtfsf(__r.__d);
    return (0);
}
#else
    return __feraiseexcept_spe(__fexcepts_to_spe (excepts));
#endif

int
fetestexcept(int __excepts) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    return (__r.__bits.__reg & __excepts);
#else
    unsigned long f;

    /* Get the current state.  */
    f = fegetenv_register ();

    return __fexcepts_from_spe (f) & __excepts;
#endif
}

int
fegetround(void) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    return (__r.__bits.__reg & _ROUND_MASK);
#else
    unsigned long fpescr;

    fpescr = fegetenv_register ();
    return fpescr & 3;
#endif
}

int
fesetround(int __round) {
#ifndef __SPE__
    union __fpscr __r;

    if (__round & ~_ROUND_MASK)
        return (-1);
    __mffs(&__r.__d);
    __r.__bits.__reg &= ~_ROUND_MASK;
    __r.__bits.__reg |= __round;
    __mtfsf(__r.__d);
    return (0);
#else
    unsigned long fpescr;

    if ((unsigned int) __round > 3)
        return 1;

    fpescr = fegetenv_register ();
    fpescr = (fpescr & ~SPEFSCR_FRMC) | (__round & 3);
    fesetenv_register (fpescr);

    return 0;
#endif
}

int
fegetenv(fenv_t * __envp) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    *__envp = __r.__bits.__reg;
    return (0);
#else
    fenv_union_t u;
    INTERNAL_SYSCALL_DECL (err);
    int r;

    r = INTERNAL_SYSCALL(prctl, err, 2, PR_GET_FPEXC, &u.l[0]);
    if (INTERNAL_SYSCALL_ERROR_P(r, err))
        return -1;

    u.l[1] = fegetenv_register ();
    *__envp = u.fenv;

    /* Success.  */
    return 0;
#endif
}

int
feholdexcept(fenv_t * __envp) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    *__envp = __r.__d;
    __r.__bits.__reg &= ~(FE_ALL_EXCEPT | _ENABLE_MASK);
    __mtfsf(__r.__d);
    return (0);
#else
    fenv_union_t u;
    INTERNAL_SYSCALL_DECL (err);
    int r;

    /* Get the current state.  */
    r = INTERNAL_SYSCALL (prctl, err, 2, PR_GET_FPEXC, &u.l[0]);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
        return -1;

    u.l[1] = fegetenv_register ();
    *__envp = u.fenv;

    /* Clear everything except for the rounding mode and trapping to the
    kernel.  */
    u.l[0] &= ~(PR_FP_EXC_DIV
                | PR_FP_EXC_OVF
                | PR_FP_EXC_UND
                | PR_FP_EXC_RES
                | PR_FP_EXC_INV);
    u.l[1] &= SPEFSCR_FRMC | (SPEFSCR_ALL_EXCEPT_ENABLE & ~SPEFSCR_FINXE);

    /* Put the new state in effect.  */
    fesetenv_register (u.l[1]);
    r = INTERNAL_SYSCALL (prctl, err, 2, PR_SET_FPEXC, u.l[0] | PR_FP_EXC_SW_ENABLE);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
        return -1;

    return 0;
#endif
}

int
fesetenv(const fenv_t *__envp) {
#ifndef __SPE__
    union __fpscr __r;

    __r.__bits.__reg = *__envp;
    __mtfsf(__r.__d);
    return (0);
#else
    fenv_union_t u;
    INTERNAL_SYSCALL_DECL(err);
    int r;

    u.fenv = *__envp;

    fesetenv_register(u.l[1]);
    r = INTERNAL_SYSCALL(prctl, err, 2, PR_SET_FPEXC, u.l[0] | PR_FP_EXC_SW_ENABLE);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
        return -1;

    /* Success.  */
    return 0;
#endif
}

int
feupdateenv(const fenv_t *__envp) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    __r.__bits.__reg &= FE_ALL_EXCEPT;
    __r.__bits.__reg |= *__envp;
    __mtfsf(__r.__d);
    return (0);
#else
    int exc;

    /* Save the currently set exceptions.  */
    exc = fegetenv_register() & SPEFSCR_ALL_EXCEPT;

    /* Install new environment.  */
    fesetenv(__envp);

    /* Raise (if appropriate) saved exceptions. */
    feraiseexcept(exc);

    /* Success.  */
    return 0;
#endif
}

/* We currently provide no external definitions of the functions below. */
int
feenableexcept(int __mask) {
#ifndef __SPE__
    union __fpscr __r;
    fenv_t __oldmask;

    __mffs(&__r.__d);
    __oldmask = __r.__bits.__reg;
    __r.__bits.__reg |= (__mask & FE_ALL_EXCEPT) >> _FPUSW_SHIFT;
    __mtfsf(__r.__d);
    return ((__oldmask & _ENABLE_MASK) << _FPUSW_SHIFT);
#else
    unsigned int result = 0, pflags, r;
    INTERNAL_SYSCALL_DECL(err);

    r = INTERNAL_SYSCALL(prctl, err, 2, PR_GET_FPEXC, &pflags);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
        return -1;

    /* Save old enable bits.  */
    result = __fexcepts_from_prctl(pflags);

    pflags |= __fexcepts_to_prctl(__mask);
    r = INTERNAL_SYSCALL (prctl, err, 2, PR_SET_FPEXC, pflags | PR_FP_EXC_SW_ENABLE);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
        return -1;

    /* If enabling signals for "inexact", also enable trapping to the
    kernel.  */
    if ((__mask & FE_INEXACT) != 0) {
        unsigned long fpescr;

        fpescr = fegetenv_register ();
        fpescr |= SPEFSCR_FINXE;
        fesetenv_register (fpescr);
    }

    return result;
#endif
}

int
fedisableexcept(int __mask) {
#ifndef __SPE__
    union __fpscr __r;
    fenv_t __oldmask;

    __mffs(&__r.__d);
    __oldmask = __r.__bits.__reg;
    __r.__bits.__reg &= ~((__mask & FE_ALL_EXCEPT) >> _FPUSW_SHIFT);
    __mtfsf(__r.__d);
    return ((__oldmask & _ENABLE_MASK) << _FPUSW_SHIFT);
#else
    int result = 0, pflags, r;
    INTERNAL_SYSCALL_DECL(err);

    r = INTERNAL_SYSCALL(prctl, err, 2, PR_GET_FPEXC, &pflags);
    if (INTERNAL_SYSCALL_ERROR_P(r, err))
        return -1;

    /* Save old enable bits.  */
    result = __fexcepts_from_prctl(pflags);

    pflags &= ~__fexcepts_to_prctl (__mask);
    r = INTERNAL_SYSCALL(prctl, err, 2, PR_SET_FPEXC, pflags | PR_FP_EXC_SW_ENABLE);
    if (INTERNAL_SYSCALL_ERROR_P (r, err))
    return -1;

    /* If disabling signals for "inexact", also disable trapping to the
    kernel.  */
    if ((__mask & FE_INEXACT) != 0) {
        unsigned long fpescr;

        fpescr = fegetenv_register ();
        fpescr &= ~SPEFSCR_FINXE;
        fesetenv_register (fpescr);
    }

    return result;
#endif
}

int
fegetexcept(void) {
#ifndef __SPE__
    union __fpscr __r;

    __mffs(&__r.__d);
    return ((__r.__bits.__reg & _ENABLE_MASK) << _FPUSW_SHIFT);
#else
    int result = 0, pflags, r;
    INTERNAL_SYSCALL_DECL (err);

    r = INTERNAL_SYSCALL(prctl, err, 2, PR_GET_FPEXC, &pflags);
    if (INTERNAL_SYSCALL_ERROR_P(r, err))
        return -1;

    result = __fexcepts_from_prctl(pflags);

    return result;
#endif
}
