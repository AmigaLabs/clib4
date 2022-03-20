/*
 * $Id: socket_ioctl.c,v 1.14 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */

#include <sys/ioctl.h>

int 
ioctl(int sockfd, int request, ... /* char *arg */)
{
	va_list arg;
	char *param;
	struct fd *fd = NULL;
	int result = ERROR;

	ENTER();

	SHOWVALUE(sockfd);
	SHOWVALUE(request);

	assert(__SocketBase != NULL);

	assert(sockfd >= 0 && sockfd < __num_fd);
	assert(__fd[sockfd] != NULL);
	assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IN_USE));
	assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

	fd = __get_file_descriptor_socket(sockfd);
	if (fd == NULL)
		goto out;

	__fd_lock(fd);

	va_start(arg, request);
	param = va_arg(arg, char *);
	va_end(arg);

	SHOWPOINTER(param);

	result = __IoctlSocket(fd->fd_Socket, request, param);

	if (result == 0)
	{
		const int *option = (const int *)param;

		if (request == (int)FIONBIO)
		{
			if ((*option) != 0)
				SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
			else
				CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
		}
		else if (request == (int)FIOASYNC)
		{
			if ((*option) != 0)
				SET_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
			else
				CLEAR_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
		}
	}

out:

	__fd_unlock(fd);

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
