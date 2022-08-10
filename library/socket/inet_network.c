/*
 * $Id: socket_inet_network.c,v 1.6 2022-08-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

in_addr_t
inet_network(const char *cp) {
    return ntohl(inet_addr(cp));
}
