/*
 * $Id: socket_connect.c,v 1.9 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
connect(int sockfd, const struct sockaddr *name, socklen_t namelen) {
    struct fd *fd;
    int result = ERROR;
    struct sockaddr_storage sst;
    int type = 0;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWVALUE(namelen);

    assert(name != NULL);
    assert(__SocketBase != NULL);

    if (name == NULL) {
        SHOWMSG("invalid name parameter");

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

    if (get_inet_addr(name, namelen, &sst, &namelen, &type, fd->fd_Secret) == ERROR)
        goto out;

    if (get_addr_family(sockfd) == AF_LOCAL) {
        if (get_socket_type(sockfd) != type) {
            return ERROR;
        }

        set_peer_sun_path(sockfd, (const char *) name->sa_data);

        /* Don't move af_local_set_cred into af_local_connect which may be called
           via select, possibly running under another identity.  Call early here,
           because af_local_connect is called in wait_for_events. */
        if (get_socket_type(sockfd) == SOCK_STREAM)
            af_local_set_cred(sockfd);
    }

    result = __connect(fd->fd_Socket, (struct sockaddr *) name, namelen);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
