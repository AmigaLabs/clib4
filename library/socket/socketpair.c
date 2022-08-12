/*
 * $Id: socket_socketpair.c,v 1.0 2021-01-24 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
socketpair(int domain, int type, int protocol, int socks[2])
{
    union
    {
        struct sockaddr_in inaddr;
        struct sockaddr addr;
    } a;

    int listener;
    socklen_t addrlen = sizeof(a.inaddr);
    int reuse = 1;
    (void)domain;
    (void)type;
    (void)protocol;

    ENTER();

    __set_errno(0);

    if (socks == 0) {
        __set_errno(EINVAL);
        RETURN(ERROR);
        return ERROR;
    }

    socks[0] = socks[1] = -1;

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        RETURN(ERROR);
        return ERROR;
    }

    memset(&a, 0, sizeof(a));
    a.inaddr.sin_family = AF_INET;
    a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.inaddr.sin_port = 0;

    for (;;) {
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, (int) sizeof(reuse)) == -1)
            break;
        if (bind(listener, &a.addr, sizeof(a.inaddr)) == -1)
            break;
        if (getsockname(listener, &a.addr, &addrlen) == -1)
            break;
        if (listen(listener, 1) == -1)
            break;
        socks[0] = socket(AF_INET, SOCK_STREAM, 0);
        if (socks[0] == -1)
            break;
        if (connect(socks[0], &a.addr, sizeof(a.inaddr)) == -1)
            break;
        socks[1] = accept(listener, NULL, NULL);
        if (socks[1] == -1)
            break;

        close(listener);
        RETURN(OK);
        return OK;
    }

    __set_errno(EFAULT);
    close(listener);
    close(socks[0]);
    close(socks[1]);

    RETURN(ERROR);
    return ERROR;
}
