/*
 * $Id: socket_sendto.c,v 1.11 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
sendto(int sockfd, const void *buff, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(buff);
    SHOWVALUE(len);
    SHOWVALUE(flags);
    SHOWPOINTER(to);
    SHOWVALUE(tolen);

    assert(buff != NULL && to != NULL);
    DECLARE_SOCKETBASE();

    if (buff == NULL || to == NULL) {
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

    result = __sendto(fd->fd_Socket, (void *) buff, len, flags, (struct sockaddr *) to, tolen);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
