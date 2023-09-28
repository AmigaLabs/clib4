/*
 * $Id: math_fabs_spe.S,v 1.00 2023-07-13 12:04:23 clib4devs Exp $
*/

#include "ppc-asm.h"

#ifdef __SPE__
FUNC_START(fabs)
	efdabs	f1, f1
	blr
FUNC_END(fabs)
#endif