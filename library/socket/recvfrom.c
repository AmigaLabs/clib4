/*
 * $Id: socket_recvfrom.c,v 1.9 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

ssize_t
recvfrom(int sockfd, void *buff, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(buff);
    SHOWVALUE(len);
    SHOWVALUE(flags);
    SHOWPOINTER(from);
    SHOWPOINTER(fromlen);

    DECLARE_SOCKETBASE();

    if (buff == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    assert(sockfd >= 0 && sockfd < __clib4->__num_fd);
    assert(__clib4->__fd[sockfd] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IN_USE));
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

    fd = __get_file_descriptor_socket(__clib4, sockfd);
    if (fd == NULL)
        goto out;

    result = __recvfrom(fd->fd_Socket, buff, len, flags, from, fromlen);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
