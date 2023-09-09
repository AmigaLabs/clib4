/*
 * $Id: math_s_lrintf_spe.S,v 1.00 2023-07-19 12:04:23 clib2devs Exp $
*/

#include "ppc-asm.h"

#ifdef __SPE__
FUNC_START(lrintf)
    /* long int [r3] lrintf (float [r3] x) ;  */
	efsctsi r3,r3
	blr
FUNC_END(lrintf)
#endif