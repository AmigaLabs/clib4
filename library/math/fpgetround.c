/*
 * $Id: fpgetround.c,v 1.0 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>

fp_rnd_t
fpgetround() {
#ifndef __SPE__
    uint64_t fpscr;

    __asm__("mffs %0" : "=f"(fpscr));
    return ((fp_rnd_t)(fpscr & 0x3));
#else
    return 0;
#endif
}
