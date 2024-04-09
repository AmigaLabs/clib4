/*
 * $Id: math_init_exit.c,v 1.20 2024-04-09 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#include <fpu_control.h>

static void
__setfpucw(fpu_control_t set) {
    fpu_control_t cw;

    /* Fetch the current control word.  */
    _FPU_GETCW (cw);

    /* Preserve the reserved bits, and set the rest as the user
       specified (or the default, if the user gave zero).  */
    cw &= _FPU_RESERVED;
    cw |= set & ~_FPU_RESERVED;

    _FPU_SETCW (cw);
}

void _math_init(void) {
    union ieee_single *single_x;
    struct _clib4 *__clib4 = __CLIB4;

	ENTER();

    /* Finally, fill in the constants behind INFINITY and NAN. */
    single_x = (union ieee_single *) &__clib4->__infinity;
    single_x->raw[0] = 0x7f800000;

    single_x = (union ieee_single *) &__clib4->__nan;
    single_x->raw[0] = 0x7fc00001;

	LEAVE();
}
