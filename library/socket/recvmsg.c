/*
 * $Id: socket_recvmsg.c,v 1.7 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int 
recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	struct fd *fd;
	int result = ERROR;

	ENTER();

	SHOWVALUE(sockfd);
	SHOWPOINTER(msg);
	SHOWVALUE(flags);

	assert(msg != NULL);
	assert(__SocketBase != NULL);

    if (msg == NULL)
    {
        SHOWMSG("invalid msg parameter");

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

	result = __recvmsg(fd->fd_Socket, msg, flags);

out:

    __check_abort();

	RETURN(result);
	return (result);
}
