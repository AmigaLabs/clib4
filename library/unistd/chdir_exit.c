/*
 * $Id: unistd_chdir_exit.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/* If the program's current directory was changed, here is where
   we find out about it. */
BPTR NOCOMMON __original_current_directory;
BOOL NOCOMMON __current_directory_changed;
BOOL NOCOMMON __unlock_current_directory;

/****************************************************************************/

CLIB_DESTRUCTOR(__chdir_exit)
{
	ENTER();

	if(__current_directory_changed)
	{
		BPTR old_dir;

		old_dir = CurrentDir(__original_current_directory);
		__original_current_directory = ZERO;

		if(__unlock_current_directory)
		{
			UnLock(old_dir);

			__unlock_current_directory = FALSE;
		}

		__current_directory_changed = FALSE;
	}

	LEAVE();
}
