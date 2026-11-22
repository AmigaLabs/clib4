/*
 * $Id: math_fabs_spe.S,v 1.00 2023-07-13 12:04:23 clib4devs Exp $
*/

#include "ppc-asm.h"

#ifdef __SPE__
FUNC_START(fabs)
    /* double [r3:r4] fabs (double [r3:r4] x) ;  clear the sign bit */
	clrlwi	r3,r3,1
	blr
FUNC_END(fabs)
#endif