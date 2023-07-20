/*
 * $Id: fpgetmask.c,v 1.0 2023-07-20 19:09:00 clib2devs Exp $
*/

#include <sys/types.h>
#include <ieeefp.h>

#ifndef _SOFT_FLOAT
fp_except_t
fpgetmask() {
    uint64_t fpscr;

    __asm__("mffs %0" : "=f"(fpscr));
    return ((fp_except_t)((fpscr >> 3) & 0x1f));
}
#endif