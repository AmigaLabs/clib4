/*
 * $Id: fenv.c,v 1.1 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <fenv.h>

#ifdef __GNUC_GNU_INLINE__
#error "This file must be compiled with C99 'inline' semantics"
#endif

#ifdef __SPE__
#include <sys/spr.h>
const fenv_t __fe_dfl_env = SPEFSCR_DFLT;
#else
const fenv_t __fe_dfl_env = 0x00000000;
#endif

extern inline int feclearexcept(int __excepts);
extern inline int fegetexceptflag(fexcept_t *__flagp, int __excepts);
extern inline int fesetexceptflag(const fexcept_t *__flagp, int __excepts);
#ifndef __SPE__
extern inline int feraiseexcept(int __excepts);
#endif
extern inline int fetestexcept(int __excepts);
extern inline int fegetround(void);
extern inline int fesetround(int __round);
extern inline int fegetenv(fenv_t *__envp);
extern inline int feholdexcept(fenv_t *__envp);
extern inline int fesetenv(const fenv_t *__envp);
extern inline int feupdateenv(const fenv_t *__envp);

#ifdef __SPE__
#define	PMAX	0x7f7fffff
#define	PMIN	0x00800000
int	feraiseexcept(int __excepts) {
	uint32_t spefscr;

	spefscr = mfspr(SPR_SPEFSCR);
	mtspr(SPR_SPEFSCR, spefscr | (__excepts & FE_ALL_EXCEPT));

	if (__excepts & FE_INVALID)
		__asm __volatile ("efsdiv %0, %0, %1" :: "r"(0), "r"(0));
	if (__excepts & FE_DIVBYZERO)
		__asm __volatile ("efsdiv %0, %0, %1" :: "r"(1.0f), "r"(0));
	if (__excepts & FE_UNDERFLOW)
		__asm __volatile ("efsmul %0, %0, %0" :: "r"(PMIN));
	if (__excepts & FE_OVERFLOW)
		__asm __volatile ("efsadd %0, %0, %0" :: "r"(PMAX));
	if (__excepts & FE_INEXACT)
		__asm __volatile ("efssub %0, %0, %1" :: "r"(PMIN), "r"(1.0f));
	return (0);
}
#endif