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
	BOOL success = FALSE;

	ENTER();

    /* Clear fenv flags */
    feclearexcept(FE_ALL_EXCEPT);
    __setfpucw(_FPU_DEFAULT);

    success = TRUE;

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
