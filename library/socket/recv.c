/*
 * $Id: socket_recv.c,v 1.7 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
recv(int sockfd, void *buff, size_t nbytes, int flags) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

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

    fd = __get_file_descriptor_socket(__clib4, sockfd);
    if (fd == NULL)
        goto out;

    result = __recv(fd->fd_Socket, buff, (LONG) nbytes, flags);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
