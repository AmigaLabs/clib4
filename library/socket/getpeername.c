/*
 * $Id: socket_getpeername.c,v 1.8 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
getpeername(int sockfd, struct sockaddr *name, socklen_t *namelen) {
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWPOINTER(namelen);

    assert(name != NULL && namelen != NULL);
    assert(__SocketBase != NULL);

    if (name == NULL || namelen == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    assert(sockfd >= 0 && sockfd < __CLIB2->__num_fd);
    assert(__CLIB2->__fd[sockfd] != NULL);
    assert(FLAG_IS_SET(__CLIB2->__fd[sockfd]->fd_Flags, FDF_IN_USE));
    assert(FLAG_IS_SET(__CLIB2->__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

    fd = __get_file_descriptor_socket(sockfd);
    if (fd == NULL)
        goto out;

    result = __getpeername(fd->fd_Socket, name, namelen);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
