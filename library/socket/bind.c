/*
 * $Id: socket_bind.c,v 1.8 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

static bool uxPortScan(const void *socket, void *port) {
    const struct UnixSocket *unixSocket = socket;
    const int socketPort = *((int *)port);
    if (unixSocket->port == socketPort)
        return false;
    return true;
}

int
bind(int sockfd, const struct sockaddr *name, socklen_t namelen) {
    struct fd *fd;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWVALUE(namelen);

    assert(name != NULL);
    DECLARE_SOCKETBASE();

    if (name == NULL) {
        SHOWMSG("invalid name parameter");

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

    if (((struct sockaddr_un *) name)->sun_family == AF_LOCAL) {
        struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
        if (res) {
            const char *socketName = ((struct sockaddr_un *) name)->sun_path;
            struct sockaddr_in serv_addr;
            int reuse = 1;

            /* Check if an unix socket is already bound with same name */
            struct UnixSocket newSock;
            memset(&newSock, 0, sizeof(struct UnixSocket));
            strncpy(newSock.name, socketName, PATH_MAX);
            if (hashmap_get(res->uxSocketsMap, &newSock) != NULL) {
                if (setsockopt(fd->fd_Socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, (int) sizeof(reuse)) == -1) {
                    __set_errno(EADDRINUSE);
                    goto out;
                }
            }

            /* Create AF_INET structure */
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            /* Find an empty port */
            int port;
            for (port = START_UX_LOCAL_PORTS; port < 65535 - START_UX_LOCAL_PORTS; port++) {
                if (hashmap_scan(res->uxSocketsMap, uxPortScan, &port)) {
                    break;
                }
            }
            serv_addr.sin_port = htons(port);
            /* Insert unix socket filename and port into unix sockets map */
            newSock.port = port;
            newSock.fd = fd;
            hashmap_set(res->uxSocketsMap, &newSock);
            result = __bind(fd->fd_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        }
        else
            __set_errno(EFAULT);
    }
    else
        result = __bind(fd->fd_Socket, (struct sockaddr *) name, namelen);


out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
