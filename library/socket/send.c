/*
 * $Id: socket_send.c,v 1.9 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int 
send(int sockfd, const void *buff, size_t nbytes, int flags)
{
	struct fd *fd;
	int result = ERROR;

	ENTER();

	SHOWVALUE(sockfd);
	SHOWPOINTER(buff);
	SHOWVALUE(nbytes);
	SHOWVALUE(flags);

	assert((int)nbytes >= 0);
	assert(buff != NULL);
	assert(__SocketBase != NULL);

    if (buff == NULL)
    {
        SHOWMSG("invalid buffer parameter");

        __set_errno(EFAULT);
        goto out;
    }

	assert(sockfd >= 0 && sockfd < __num_fd);
	assert(__fd[sockfd] != NULL);
	assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IN_USE));
	assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

	fd = __get_file_descriptor_socket(sockfd);
	if (fd == NULL)
		goto out;

	result = __send(fd->fd_Socket, (void *)buff, (LONG)nbytes, flags);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
