/*
 * $Id: math_init_exit.c,v 1.19 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

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

MATH_CONSTRUCTOR(math_init) {
    union ieee_single *single_x;
    struct _clib2 *__clib2 = __CLIB2;

	ENTER();

    /* Finally, fill in the constants behind INFINITY and NAN. */
    single_x = (union ieee_single *) &__clib2->__infinity;
    single_x->raw[0] = 0x7f800000;

    single_x = (union ieee_single *) &__clib2->__nan;
    single_x->raw[0] = 0x7fc00001;

	SHOWVALUE(success);
	LEAVE();

    CONSTRUCTOR_SUCCEED();
}
