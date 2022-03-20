/*
 * $Id: unistd_execve_exit.c,v 1.1 2006-08-06 08:15:42 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

/* This is a stub which you may need to override with your own implementation.
   It is supposed to perform any cleanup duties for the currently running
   program and then return control to the shell. However, it is perfectly
   safe not to call exit() here and merely return immediately. In this case
   the execve() function will simply return to the caller of the function. */
void
__execve_exit(int return_code)
{
	exit(return_code);
}
