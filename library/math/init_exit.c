/*
 * $Id: math_init_exit.c,v 1.19 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

float NOCOMMON __infinity;
float NOCOMMON __nan;
float NOCOMMON __huge_val_float;
long double NOCOMMON __huge_val_long_double;
double NOCOMMON __huge_val;

MATH_CONSTRUCTOR(math_init)
{
    union ieee_double *double_x;
    union ieee_single *single_x;
    union ieee_long_double *x;

	BOOL success = FALSE;

	ENTER();

    /* Now fill in HUGE_VAL and HUGE_VALF, respectively. TODO:
       also take care of HUGE_VALL. */

    /* Exponent = +126, Mantissa = 8,388,607 */
    single_x = (union ieee_single *)&__huge_val_float;
    single_x->raw[0] = 0x7f7fffff;

    /* Exponent = +1022, Mantissa = 4,503,599,627,370,495 */
    double_x = (union ieee_double *)&__huge_val;
    double_x->raw[0] = 0x7fefffff;
    double_x->raw[1] = 0xffffffff;

    x = (union ieee_long_double *)&__huge_val_long_double;
    /* Exponent = +32766, Mantissa = 18,446,744,073,709,551,615 */
    x->raw[0] = 0x7ffe0000;
    x->raw[1] = 0xffffffff;
    x->raw[2] = 0xffffffff;

    /* Finally, fill in the constants behind INFINITY and NAN. */
    single_x = (union ieee_single *)&__infinity;
    single_x->raw[0] = 0x7f800000;

    single_x = (union ieee_single *)&__nan;
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
