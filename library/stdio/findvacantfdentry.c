/*
 * $Id: stdio_findvacantfdentry.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef NDEBUG

BOOL
__is_valid_fd(struct fd * fd)
{
	BOOL result = FALSE;

	__stdio_lock();

	if(__fd != NULL && __num_fd > 0)
	{
		int i;

		for(i = 0 ; i < __num_fd ; i++)
		{
			if(__fd[i] == fd)
			{
				result = TRUE;
				break;
			}
		}
	}

	__stdio_unlock();

	return(result);
}

#endif /* NDEBUG */

int
__find_vacant_fd_entry(void)
{
	int result = ERROR;
	int i;

	assert( __fd != NULL || __num_fd == 0 );

	for(i = 0 ; i < __num_fd ; i++)
	{
		if(FLAG_IS_CLEAR(__fd[i]->fd_Flags,FDF_IN_USE))
		{
			result = i;
			break;
		}
	}

	return(result);
}
