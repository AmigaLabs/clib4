/*
 * $Id: socket_getsockopt.c,v 1.8 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWVALUE(level);
    SHOWVALUE(optname);
    SHOWPOINTER(optval);
    SHOWPOINTER(optlen);

    assert(optval != NULL && optlen != NULL);
    assert(__SocketBase != NULL);

    if (optval == NULL || optlen == NULL) {
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

    result = __getsockopt(fd->fd_Socket, level, optname, optval, optlen);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
