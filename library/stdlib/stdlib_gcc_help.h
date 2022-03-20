/*
 * $Id: stdlib_gcc_help.h,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_GCC_HELP_H
#define _STDLIB_GCC_HELP_H

/****************************************************************************/

/*
 * Uncomment this to build a library which contains code that helps
 * GCC to build itself. This has two effects: the shell escape character
 * "\" is enabled and the default stack size is raised to 200000 bytes
 * with an additional helper function included which can be used to override
 * the default stack size.
 */
/*#define GCC_HELP_SUPPORT*/

/****************************************************************************/

#endif /* _STDLIB_GCC_HELP_H */
