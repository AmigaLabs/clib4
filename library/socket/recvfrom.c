/*
 * $Id: socket_recvfrom.c,v 1.9 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

ssize_t
recvfrom(int sockfd, void *buff, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) {
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(buff);
    SHOWVALUE(len);
    SHOWVALUE(flags);
    SHOWPOINTER(from);
    SHOWPOINTER(fromlen);

    assert(buff != NULL && from != NULL && fromlen != NULL);
    assert(__SocketBase != NULL);

    if (buff == NULL || from == NULL || fromlen == NULL) {
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

    result = __recvfrom(fd->fd_Socket, buff, len, flags, from, fromlen);

out:

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
