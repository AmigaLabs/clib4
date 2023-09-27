/*
 * $Id: socket_inet_makeaddr.c,v 1.5 2022-08-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct in_addr
inet_makeaddr(in_addr_t net, in_addr_t host) {
    struct in_addr result;

    ENTER();

    if (net < 256) host |= net<<24;
    else if (net < 65536) host |= net<<16;
    else host |= net<<8;
    result.s_addr = host;

    __check_abort();

    RETURN(result.s_addr);
    return (result);
}
