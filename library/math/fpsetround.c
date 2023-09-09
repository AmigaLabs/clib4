/*
 * $Id: fpsetround.c,v 1.0 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>
#include <sys/spr.h>

fp_rnd_t
fpsetround(fp_rnd_t rnd_dir) {
#ifndef __SPE__
    uint64_t fpscr;
    fp_rnd_t old;

    __asm__ __volatile("mffs %0" : "=f"(fpscr));
    old = (fp_rnd_t)(fpscr & 0x3);
    fpscr = (fpscr & 0xfffffffc) | rnd_dir;
    __asm__ __volatile("mtfsf 0xff,%0"::"f"(fpscr));
    return (old);
#else
    uint32_t fpscr;
	fp_rnd_t old;

	__asm__ __volatile("mfspr %0, %1" : "=r"(fpscr) : "K"(SPR_SPEFSCR) );
	old = (fp_rnd_t)(fpscr & 0x3);
	fpscr = (fpscr & 0xfffffffc) | rnd_dir;
	__asm__ __volatile("mtspr %1, %0" :: "r"(fpscr), "K"(SPR_SPEFSCR));
	return (old);
#endif
}

