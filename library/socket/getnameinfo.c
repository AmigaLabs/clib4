/*
 * $Id: socket_getnameinfo.c,v 1.0 2021-01-13 11:31:35 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
getnameinfo(const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags) {
    int result = 0;

    /* FIXME: Support other flags. */
    if (flags & ~(NI_NUMERICHOST | NI_NUMERICSERV)) {
        result = EAI_BADFLAGS;
        goto out;
    }

    if (sa == NULL || salen < sizeof(sa->sa_family)) {
        result = EAI_FAMILY;
        goto out;
    }

    switch (sa->sa_family) {
        case AF_INET:
            if (salen < sizeof(struct sockaddr_in)) {
                result = EAI_FAMILY;
                goto out;
            }
            break;
        case AF_INET6:
            if (salen < sizeof(struct sockaddr_in6)) {
                result = EAI_FAMILY;
                goto out;
            }
            break;
        default:
            result = EAI_FAMILY;
            goto out;
    }
    if (node && nodelen > 0 && flags & NI_NUMERICHOST) {
        switch (sa->sa_family) {
            case AF_INET:
                if (!inet_ntop(AF_INET, &(((const struct sockaddr_in *) sa)->sin_addr), node, nodelen)) {
                    result = EAI_SYSTEM;
                    goto out;
                }
                break;
            case AF_INET6:
                if (!inet_ntop(AF_INET6, &(((const struct sockaddr_in6 *) sa)->sin6_addr), node, nodelen)) {
                    result = EAI_SYSTEM;
                    goto out;
                }
                break;
            default:
                result = EAI_FAMILY;
                goto out;
        }
    }

    if (service && servicelen > 0 && flags & NI_NUMERICSERV) {
        switch (sa->sa_family) {
            case AF_INET:
            case AF_INET6: {
                unsigned short int port = ntohs(((const struct sockaddr_in *) sa)->sin_port);
                if (servicelen <= (socklen_t) snprintf(service, servicelen, "%u", port)) {
                    result = EAI_OVERFLOW;
                    goto out;
                }
            }
                break;
        }
    }

out:

    if (__check_abort_enabled)
        __check_abort();

    return result;
}
