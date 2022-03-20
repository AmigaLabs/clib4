/*
 * $Id: stdlib_stacksize.c,v 1.5 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_GCC_HELP_H
#include "stdlib_gcc_help.h"
#endif /* _STDLIB_GCC_HELP_H */

/****************************************************************************/

#include "macros.h"

unsigned int NOCOMMON __stack_size;
