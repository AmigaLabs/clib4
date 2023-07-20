/*
 * $Id: fpgetround.c,v 1.0 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>

#ifndef _SOFT_FLOAT
fp_rnd_t
fpgetround() {
    uint64_t fpscr;

    __asm__("mffs %0" : "=f"(fpscr));
    return ((fp_rnd_t)(fpscr & 0x3));
}
#endif