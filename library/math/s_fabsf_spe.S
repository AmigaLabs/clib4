/*
 * $Id: math_fabsf_spe.S,v 1.00 2023-07-13 12:04:23 clib2devs Exp $
*/

#include "ppc-asm.h"

#ifdef __SPE__
FUNC_START(fabsf)
    /* float [r3] fabsf (float [r3] x) ;  */
	efsabs r3,r3
	blr
FUNC_END(fabsf)
#endif