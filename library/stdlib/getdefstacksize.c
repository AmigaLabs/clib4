/*
 * $Id: stdlib_getdefstacksize.c,v 1.4 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_GCC_HELP_H
#include "stdlib_gcc_help.h"
#endif /* _STDLIB_GCC_HELP_H */

#ifndef GCC_HELP_SUPPORT

unsigned int (* __get_default_stack_size)(void);

#endif /* GCC_HELP_SUPPORT */
