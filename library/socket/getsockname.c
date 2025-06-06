/*
 * $Id: socket_getsockname.c,v 1.8 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
getsockname(int sockfd, struct sockaddr *name, socklen_t *namelen) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWPOINTER(namelen);

    assert(name != NULL && namelen != NULL);
    DECLARE_SOCKETBASE_R(__clib4);

    if (name == NULL || namelen == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    assert(sockfd >= 0 && sockfd < __clib4->__num_fd);
    assert(__clib4->__fd[sockfd] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IN_USE));
    assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

    fd = __get_file_descriptor_socket(__clib4, sockfd);
    if (fd == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __getsockname(fd->fd_Socket, name, namelen);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
