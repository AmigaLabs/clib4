/*
 * $Id: socket_sendto.c,v 1.11 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int 
sendto(int sockfd, const void *buff, size_t len, int flags, const struct sockaddr *to, socklen_t tolen)
{
	struct fd *fd;
	int result = ERROR;

	ENTER();

	SHOWVALUE(sockfd);
	SHOWPOINTER(buff);
	SHOWVALUE(len);
	SHOWVALUE(flags);
	SHOWPOINTER(to);
	SHOWVALUE(tolen);

	assert(buff != NULL && to != NULL);
	assert(__SocketBase != NULL);

    if (buff == NULL || to == NULL)
    {
        SHOWMSG("invalid parameters");

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

	result = __sendto(fd->fd_Socket, (void *)buff, len, flags, (struct sockaddr *)to, tolen);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
