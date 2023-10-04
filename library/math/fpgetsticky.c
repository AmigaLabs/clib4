/*
 * $Id: fpgetsticky.c,v 1.0 2023-07-20 19:09:00 clib4devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>
#include <sys/spr.h>

fp_except_t
fpgetsticky() {
#ifndef __SPE__
    uint64_t fpscr;

    __asm__ __volatile("mffs %0" : "=f"(fpscr));
    return ((fp_except_t)((fpscr >> 25) & 0x1f));
#else
    uint32_t fpscr;

	__asm__ __volatile("mfspr %0, %1" : "=r"(fpscr) : "K"(SPR_SPEFSCR));
	return ((fp_except_t)((fpscr >> 25) & 0x1f));
#endif
}
