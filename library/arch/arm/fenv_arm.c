/*
 * fenv_arm.c - the floating point environment on ARM VFP (Raspberry Pi 2).
 *
 * The environment is the FPSCR register. Its layout, as the ARM architecture
 * defines it:
 *   bits 4..0    cumulative exception flags   IOC DZC OFC UFC IXC
 *   bits 12..8   exception trap enables       IOE DZE OFE UFE IXE
 *   bits 23..22  rounding mode                0 nearest, 1 +inf, 2 -inf, 3 zero
 * The flag bits are numbered as the FE_* constants in fenv.h, so the flag
 * operations are direct. The ieeefp.h mask and round functions go through the
 * symbolic FP_* names, never through fixed numbers.
 *
 * Only the ARM core moves the FPSCR (vmrs/vmsr); no VFP data register is used,
 * so the library stays soft-float. The compiler (-mfloat-abi=soft) declares
 * softvfp, which does not assemble vmrs/vmsr; the .fpu directive scopes VFP to
 * those two instructions and restores softvfp straight after.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <fenv.h>
#include <ieeefp.h>

#define FPSCR_EXC_MASK      (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)
#define FPSCR_ENABLE_SHIFT  8
#define FPSCR_ENABLE_MASK   (FPSCR_EXC_MASK << FPSCR_ENABLE_SHIFT)
#define FPSCR_RMODE_MASK    FE_TOWARDZERO

const fenv_t __fe_dfl_env = FE_TONEAREST;

static inline fenv_t get_fpscr(void)
{
    fenv_t v;
    __asm__ __volatile__(".fpu vfpv3-d16\n\tvmrs %0, fpscr\n\t.fpu softvfp" : "=r"(v));
    return v;
}

static inline void set_fpscr(fenv_t v)
{
    __asm__ __volatile__(".fpu vfpv3-d16\n\tvmsr fpscr, %0\n\t.fpu softvfp" : : "r"(v) : "memory");
}

int
fegetenv(fenv_t *envp)
{
    *envp = get_fpscr();
    return 0;
}

int
fesetenv(const fenv_t *envp)
{
    set_fpscr(*envp);
    return 0;
}

int
feholdexcept(fenv_t *envp)
{
    fenv_t cur = get_fpscr();
    *envp = cur;
    set_fpscr(cur & ~(FPSCR_EXC_MASK | FPSCR_ENABLE_MASK));
    return 0;
}

int
feupdateenv(const fenv_t *envp)
{
    int raised = fetestexcept(FE_ALL_EXCEPT);
    set_fpscr(*envp);
    return feraiseexcept(raised);
}

int
fegetexceptflag(fexcept_t *flagp, int excepts)
{
    *flagp = (fexcept_t)(get_fpscr() & (unsigned int)excepts & FE_ALL_EXCEPT);
    return 0;
}

int
fesetexceptflag(const fexcept_t *flagp, int excepts)
{
    fenv_t cur = get_fpscr();
    unsigned int sel = (unsigned int)excepts & FE_ALL_EXCEPT;
    set_fpscr((cur & ~sel) | (*flagp & sel));
    return 0;
}

int
feclearexcept(int excepts)
{
    set_fpscr(get_fpscr() & ~((unsigned int)excepts & FE_ALL_EXCEPT));
    return 0;
}

int
fetestexcept(int excepts)
{
    return (int)(get_fpscr() & (unsigned int)excepts & FE_ALL_EXCEPT);
}

/* The flags are set, but no trap is taken: an enabled exception raised here
 * does not deliver SIGFPE. Programs on this system do not enable the traps. */
int
feraiseexcept(int excepts)
{
    set_fpscr(get_fpscr() | ((unsigned int)excepts & FE_ALL_EXCEPT));
    return 0;
}

int
fegetround(void)
{
    return (int)(get_fpscr() & FPSCR_RMODE_MASK);
}

int
fesetround(int round)
{
    fenv_t cur;

    if (((unsigned int)round & ~FPSCR_RMODE_MASK) != 0)
        return -1;
    cur = get_fpscr();
    set_fpscr((cur & ~FPSCR_RMODE_MASK) | ((unsigned int)round & FPSCR_RMODE_MASK));
    return 0;
}

/* ---- ieeefp.h: the mask, round and sticky interface ---- */

static fp_except_t
fpscr_to_except(unsigned int bits)
{
    fp_except_t e = 0;

    if (bits & FE_INVALID)   e |= FP_X_INV;
    if (bits & FE_DIVBYZERO) e |= FP_X_DZ;
    if (bits & FE_OVERFLOW)  e |= FP_X_OFL;
    if (bits & FE_UNDERFLOW) e |= FP_X_UFL;
    if (bits & FE_INEXACT)   e |= FP_X_IMP;
    return e;
}

static unsigned int
except_to_fpscr(fp_except_t e)
{
    unsigned int bits = 0;

    if (e & FP_X_INV) bits |= FE_INVALID;
    if (e & FP_X_DZ)  bits |= FE_DIVBYZERO;
    if (e & FP_X_OFL) bits |= FE_OVERFLOW;
    if (e & FP_X_UFL) bits |= FE_UNDERFLOW;
    if (e & FP_X_IMP) bits |= FE_INEXACT;
    return bits;
}

fp_except_t
fpgetmask(void)
{
    return fpscr_to_except((unsigned int)(get_fpscr() & FPSCR_ENABLE_MASK) >> FPSCR_ENABLE_SHIFT);
}

fp_except_t
fpsetmask(fp_except_t mask)
{
    fenv_t cur = get_fpscr();
    fp_except_t old = fpscr_to_except((unsigned int)(cur & FPSCR_ENABLE_MASK) >> FPSCR_ENABLE_SHIFT);

    set_fpscr((cur & ~FPSCR_ENABLE_MASK) | (except_to_fpscr(mask) << FPSCR_ENABLE_SHIFT));
    return old;
}

fp_except_t
fpgetsticky(void)
{
    return fpscr_to_except(get_fpscr() & FPSCR_EXC_MASK);
}

fp_rnd_t
fpgetround(void)
{
    switch (get_fpscr() & FPSCR_RMODE_MASK) {
    case FE_UPWARD:   return FP_RP;
    case FE_DOWNWARD: return FP_RM;
    case FE_TOWARDZERO: return FP_RZ;
    default:          return FP_RN;
    }
}

fp_rnd_t
fpsetround(fp_rnd_t rnd_dir)
{
    fp_rnd_t old = fpgetround();
    unsigned int bits;

    switch (rnd_dir) {
    case FP_RP: bits = FE_UPWARD;     break;
    case FP_RM: bits = FE_DOWNWARD;   break;
    case FP_RZ: bits = FE_TOWARDZERO; break;
    default:    bits = FE_TONEAREST;  break;
    }
    set_fpscr((get_fpscr() & ~FPSCR_RMODE_MASK) | bits);
    return old;
}
