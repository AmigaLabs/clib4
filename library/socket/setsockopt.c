/*
 * $Id: socket_setsockopt.c,v 1.10 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWVALUE(level);
    SHOWVALUE(optname);
    SHOWPOINTER(optval);
    SHOWVALUE(optlen);

    assert(optval != NULL);
    DECLARE_SOCKETBASE();

    if (optval == NULL) {
        SHOWMSG("invalid optval parameter");

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

    result = __setsockopt(fd->fd_Socket, level, optname, (void *) optval, optlen);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
