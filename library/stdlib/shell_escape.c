/*
 * $Id: stdlib_shell_escape.c,v 1.5 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_GCC_HELP_H
#include "stdlib_gcc_help.h"
#endif /* _STDLIB_GCC_HELP_H */

/****************************************************************************/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

/* Quick "hack" to help GCC compile itself */
#if defined(GCC_HELP_SUPPORT)

/******************************************************************************/

UBYTE NOCOMMON __shell_escape_character = '\\';

/******************************************************************************/

#else

/******************************************************************************/

UBYTE NOCOMMON __shell_escape_character = '*';

/******************************************************************************/

#endif /* GCC_HELP_SUPPORT */
