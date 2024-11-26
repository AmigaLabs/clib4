/*
 * $Id: socket_connect.c,v 1.9 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include "map.h"

int
connect(int sockfd, const struct sockaddr *name, socklen_t namelen) {
    struct fd *fd;
    int result = ERROR;
    struct sockaddr *sa = name;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWPOINTER(name);
    SHOWVALUE(namelen);

    DECLARE_SOCKETBASE();

    assert(name != NULL);

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

    if (((struct sockaddr_un *) sa)->sun_family == AF_LOCAL) {
        struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
        if (res) {
            const char *socketName = ((struct sockaddr_un *) name)->sun_path;
            /* Check if we have an unix socket with this name otherwise raise an error */
            struct UnixSocket key;
            memset(&key, 0, sizeof(struct UnixSocket));
            strncpy(key.name, socketName, PATH_MAX);
            struct UnixSocket *unixSocket = hashmap_get(res->uxSocketsMap, &key);
            if (unixSocket == NULL) {
                __set_errno(ENOTCONN);
                goto out;
            }
            struct sockaddr_in serv_addr;
            /* Create AF_INET structure */
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            serv_addr.sin_port = htons(unixSocket->port);

            result = __connect(fd->fd_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        }
        else
            __set_errno(EFAULT);
    }
    else
        result = __connect(fd->fd_Socket, (struct sockaddr *) name, namelen);

out:

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
