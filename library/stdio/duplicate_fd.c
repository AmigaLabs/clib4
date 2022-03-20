/*
 * $Id: stdio_duplicate_fd.c,v 1.6 2006-10-10 13:39:26 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

void 
__duplicate_fd(struct fd *duplicate_fd, struct fd *original_fd)
{
	assert(duplicate_fd != NULL && original_fd != NULL);

	__fd_lock(original_fd);

	/* Initialize the duplicate to match the original. */
	__initialize_fd(duplicate_fd, original_fd->fd_Action, original_fd->fd_DefaultFile, original_fd->fd_Flags, original_fd->fd_Lock);

	/* Figure out where the linked list of file descriptors associated
	   with this one starts. */
	if (original_fd->fd_Original != NULL)
		duplicate_fd->fd_Original = original_fd->fd_Original;
	else
		duplicate_fd->fd_Original = original_fd;

	/* Add the duplicate at the beginning of the list. */
	duplicate_fd->fd_NextAlias = duplicate_fd->fd_Original->fd_NextAlias;
	duplicate_fd->fd_Original->fd_NextAlias = duplicate_fd;

	__fd_unlock(original_fd);
}
