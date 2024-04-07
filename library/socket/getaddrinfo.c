/*
 * $Id: socket_getaddrinfo.c,v 1.0 2021-01-13 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

static inline bool
validate_family(int family) {
    if (family == PF_INET)
        return TRUE;
    if (family == PF_INET6)
        return TRUE;
    if (family == PF_UNSPEC)
        return TRUE;
    return FALSE;
}

/* Translate name of a service location and/or a service name to set of socket addresses. */
int
getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res) {
    struct addrinfo *tmp;
    int port = 0;
    struct hostent *he;
    void *storage;
    size_t size;

    struct v6_pair {
        struct addrinfo addrinfo;
        struct sockaddr_in6 sockaddr_in6;
    };
    struct v4_pair {
        struct addrinfo addrinfo;
        struct sockaddr_in sockaddr_in;
    };

    if (hints && (hints->ai_flags & ~(AI_CANONNAME | AI_PASSIVE | AI_ADDRCONFIG)))
        /* FIXME: Support more flags. */
        return EAI_BADFLAGS;

    if (hints && !validate_family(hints->ai_family))
        return EAI_FAMILY;

    if (hints &&
        hints->ai_socktype != SOCK_STREAM && hints->ai_socktype != SOCK_DGRAM)
        /* FIXME: Support other socktype. */
        return EAI_SOCKTYPE; /* FIXME: Better return code? */

    if (!nodename) {
        if (!(hints->ai_flags & AI_PASSIVE))
            return EAI_NONAME;

        if (AI_ADDRCONFIG)
            nodename = "0.0.0.0";
        else
            nodename = (hints->ai_family == AF_INET6) ? "::" : "0.0.0.0";
    }

    if (servname) {
        struct servent *se = NULL;
        const char *proto = (hints && hints->ai_socktype == SOCK_DGRAM) ? "udp" : "tcp";

        if (hints == NULL || !(hints->ai_flags & AI_NUMERICSERV))
            /* FIXME: Use getservbyname_r if available. */
            se = getservbyname(servname, proto);

        if (!se) {
            char *c;
            if (!(*servname >= '0' && *servname <= '9'))
                return EAI_NONAME;
            port = strtoul(servname, &c, 10);
            if (*c || port > 0xffff)
                return EAI_NONAME;
            port = htons(port);
        } else
            port = se->s_port;
    }

    /* FIXME: Use gethostbyname_r if available. */
    he = gethostbyname(nodename);
    if (!he || he->h_addr_list[0] == NULL)
        return EAI_NONAME;

    switch (he->h_addrtype) {
        case PF_INET6:
            size = sizeof(struct v6_pair);
            break;
        case PF_INET:
            size = sizeof(struct v4_pair);
            break;

        default:
            return EAI_NODATA;
    }

    storage = calloc(1, size);
    if (!storage)
        return EAI_MEMORY;

    switch (he->h_addrtype) {
        case PF_INET6: {
                struct v6_pair *p = storage;
                struct sockaddr_in6 *sinp = &p->sockaddr_in6;
                tmp = &p->addrinfo;

                if (port)
                    sinp->sin6_port = port;

                if (he->h_length != sizeof(sinp->sin6_addr)) {
                    free(storage);
                    return EAI_SYSTEM; /* FIXME: Better return code?  Set errno? */
                }

                memcpy(&sinp->sin6_addr, he->h_addr_list[0], sizeof sinp->sin6_addr);

                tmp->ai_addr = (struct sockaddr *) sinp;
                tmp->ai_addrlen = sizeof *sinp;
            }
            break;
        case PF_INET: {
                struct v4_pair *p = (struct v4_pair *) storage;
                struct sockaddr_in *sinp = &p->sockaddr_in;
                tmp = &p->addrinfo;

                if (port)
                    sinp->sin_port = port;

                if (he->h_length != sizeof(sinp->sin_addr)) {
                    free(storage);
                    return EAI_SYSTEM; /* FIXME: Better return code?  Set errno? */
                }

                memcpy(&sinp->sin_addr, he->h_addr_list[0], sizeof sinp->sin_addr);

                tmp->ai_addr = (struct sockaddr *) sinp;
                tmp->ai_addrlen = sizeof *sinp;
            }
            break;

        default:
            free(storage);
            return EAI_NODATA;
    }

    if (hints && hints->ai_flags & AI_CANONNAME) {
        const char *cn;
        if (he->h_name)
            cn = (const char *) he->h_name;
        else
            cn = nodename;

        tmp->ai_canonname = strdup(cn);
        if (!tmp->ai_canonname) {
            free(storage);
            return EAI_MEMORY;
        }
    }

    tmp->ai_protocol = (hints) ? hints->ai_protocol : 0;
    tmp->ai_socktype = (hints) ? hints->ai_socktype : 0;
    tmp->ai_addr->sa_family = he->h_addrtype;
    tmp->ai_family = he->h_addrtype;

    /* FIXME: If more than one address, create linked list of addrinfo's. */

    *res = tmp;

    return 0;
}
