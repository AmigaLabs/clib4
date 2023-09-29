/*
 * $Id: socket_inet_netof.c,v 1.5 2022-08-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

in_addr_t
inet_netof(struct in_addr in) {
    in_addr_t result;

    ENTER();

    uint32_t h = in.s_addr;
    if (h>>24 < 128) return h >> 24;
    if (h>>24 < 192) return h >> 16;
    result = h >> 8;

    __check_abort();

    RETURN(result);
    return (result);
}
