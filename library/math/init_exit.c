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
	BOOL success = FALSE;

	ENTER();

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
