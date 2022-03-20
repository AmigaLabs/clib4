/*
 * $Id: socket_get_descriptor.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct fd *
__get_file_descriptor_socket(int socket_descriptor)
{
	struct fd *result = NULL;
	struct fd *fd;

	fd = __get_file_descriptor(socket_descriptor);
	if (fd == NULL)
	{
		__set_errno(EBADF);
		goto out;
	}

	if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SOCKET))
	{
		__set_errno(ENOTSOCK);
		goto out;
	}

	result = fd;

out:

	return (result);
}
