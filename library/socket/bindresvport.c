/*
 * $Id: socket_bindresvport.c,v 1.0 2022-04-14 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

/*
 * Bind a socket to a privileged port for whatever protocol.
 */
static int
bindresvport_sa(int sd, struct sockaddr *sa) {
    int old, error, af;
    struct sockaddr_storage myaddr;
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;
    int proto, portrange, portlow;
    uint16_t port;
    uint32_t salen;

    if (sa == NULL) {
        salen = sizeof(myaddr);
        sa = (struct sockaddr *) &myaddr;

        if (getsockname(sd, sa, (socklen_t *) &salen) == -1)
            return -1;    /* errno is correctly set */

        af = sa->sa_family;
        memset(&myaddr, 0, salen);
    } else
        af = sa->sa_family;

    if (af == AF_INET) {
        proto = IPPROTO_IP;
        portrange = IP_PORTRANGE;
        portlow = IP_PORTRANGE_LOW;
        sin = (struct sockaddr_in *) sa;
        salen = sizeof(struct sockaddr_in);
        port = sin->sin_port;
    } else if (af == AF_INET6) {
        proto = IPPROTO_IPV6;
        portrange = IPV6_PORTRANGE;
        portlow = IPV6_PORTRANGE_LOW;
        sin6 = (struct sockaddr_in6 *) sa;
        salen = sizeof(struct sockaddr_in6);
        port = sin6->sin6_port;
    } else {
        errno = EPFNOSUPPORT;
        return (-1);
    }
    sa->sa_family = af;
    sa->sa_len = salen;

    if (port == 0) {
        uint32_t oldlen = sizeof(old);

        error = getsockopt(sd, proto, portrange, &old, (socklen_t *) &oldlen);
        if (error < 0)
            return (error);

        error = setsockopt(sd, proto, portrange, &portlow, sizeof(portlow));
        if (error < 0)
            return (error);
    }

    error = bind(sd, sa, salen);

    if (port == 0) {
        int saved_errno = errno;

        if (error) {
            if (setsockopt(sd, proto, portrange, &old, sizeof(old)) < 0)
                __set_errno(saved_errno);
            return (error);
        }

        if (sa != (struct sockaddr *) &myaddr) {
            /* Hmm, what did the kernel assign... */
            if (getsockname(sd, sa, (socklen_t *) &salen) < 0)
                __set_errno(saved_errno);
            return (error);
        }
    }
    return (error);
}

/*
 * Bind a socket to a privileged IP port
 */
int
bindresvport(int sd, struct sockaddr_in *sin)
{
    return bindresvport_sa(sd, (struct sockaddr *) sin);
}