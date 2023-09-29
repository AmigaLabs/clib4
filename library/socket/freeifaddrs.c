/*
 * $Id: getifaddrs.c,v 1.0 2022-02-10 17:02:58 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <ifaddrs.h>
#include <net/if.h>

void
freeifaddrs(struct ifaddrs *ifap)
{
    struct ifaddrs *c, *n;

    c = ifap;
    while (c != NULL) {
        n = c->ifa_next;
        free(c);
        c = n;
    }
}