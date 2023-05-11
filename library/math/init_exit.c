/*
 * $Id: math_init_exit.c,v 1.19 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

MATH_CONSTRUCTOR(math_init) {
    struct _clib2 *__clib2 = __CLIB2;
    union ieee_single *single_x;

	BOOL success = FALSE;

	ENTER();

    /* Finally, fill in the constants behind INFINITY and NAN. */
    single_x = (union ieee_single *)&__clib2->__infinity;
    single_x->raw[0] = 0x7f800000;

    single_x = (union ieee_single *)&__clib2->__nan;
    single_x->raw[0] = 0x7fc00001;

    /* Clear fenv flags */
    feclearexcept(FE_ALL_EXCEPT);

    success = TRUE;

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
