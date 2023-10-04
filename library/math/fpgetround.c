/*
 * $Id: fpgetround.c,v 1.0 2023-07-20 19:09:00 clib4devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>
#include <sys/spr.h>

fp_rnd_t
fpgetround() {
#ifndef __SPE__
    uint64_t fpscr;

    __asm__("mffs %0" : "=f"(fpscr));
    return ((fp_rnd_t)(fpscr & 0x3));
#else
    uint32_t fpscr;

	__asm__ __volatile("mfspr %0, %1" : "=r"(fpscr) : "K"(SPR_SPEFSCR));
	return ((fp_rnd_t)(fpscr & 0x3));
#endif
}
