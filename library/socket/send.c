/*
 * $Id: socket_send.c,v 1.10 2024-07-12 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
send(int sockfd, const void *buff, size_t nbytes, int flags) {
    return __send_r(__CLIB4, sockfd, buff, nbytes, flags);
}

int
__send_r(struct _clib4 *__clib4, int sockfd, const void *buff, size_t nbytes, int flags) {
    struct fd *fd;
    int result = ERROR;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(buff);
    SHOWVALUE(nbytes);
    SHOWVALUE(flags);

    assert((int) nbytes >= 0);
    assert(buff != NULL);
    DECLARE_SOCKETBASE();

    if (buff == NULL) {
        SHOWMSG("invalid buffer parameter");

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

    result = __send(fd->fd_Socket, (void *) buff, (LONG) nbytes, flags);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
