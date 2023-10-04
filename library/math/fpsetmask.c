/*
 * $Id: fpsetmask.c,v 1.0 2023-07-20 19:09:00 clib4devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>
#include <sys/spr.h>

fp_except_t
fpsetmask(fp_except_t mask) {
#ifndef __SPE__
    uint64_t fpscr;
    fp_except_t old;

    __asm__("mffs %0" : "=f"(fpscr));
    old = (fp_except_t)((fpscr >> 3) & 0x1f);
    fpscr = (fpscr & 0xffffff07) | ((mask & 0x1f) << 3);
    __asm__ __volatile("mtfsf 0xff,%0"::"f"(fpscr));
    return (old);
#else
    uint32_t fpscr;
	fp_rnd_t old;

	__asm__ __volatile("mfspr %0, %1" : "=r"(fpscr) : "K"(SPR_SPEFSCR));
	old = (fp_rnd_t)((fpscr >> 3) & 0x1f);
	fpscr = (fpscr & 0xffffff07) | (mask << 3);
	__asm__ __volatile("mtspr %1,%0" :: "r"(fpscr), "K"(SPR_SPEFSCR));
	return (old);
#endif
}
