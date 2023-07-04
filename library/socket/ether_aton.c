/*
 * $Id: socket_ether_aton.c,v 1.0 2023-06-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct ether_addr *
ether_aton(const char *x) {
    static struct ether_addr a;
    return ether_aton_r(x, &a);
}