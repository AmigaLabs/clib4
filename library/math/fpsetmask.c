/*
 * $Id: fpsetmask.c,v 1.0 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>

#ifndef _SOFT_FLOAT
fp_except_t
fpsetmask(fp_except_t mask) {
    uint64_t fpscr;
    fp_except_t old;

    __asm__("mffs %0" : "=f"(fpscr));
    old = (fp_except_t)((fpscr >> 3) & 0x1f);
    fpscr = (fpscr & 0xffffff07) | ((mask & 0x1f) << 3);
    __asm__ __volatile("mtfsf 0xff,%0"::"f"(fpscr));
    return (old);
}
#endif