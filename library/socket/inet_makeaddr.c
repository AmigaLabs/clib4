/*
 * $Id: socket_inet_makeaddr.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct in_addr
inet_makeaddr(in_addr_t net, in_addr_t host) {
    struct in_addr result;

    ENTER();

    assert(__SocketBase != NULL);

    result.s_addr = __Inet_MakeAddr((ULONG) net, (ULONG) host);

    __check_abort();

    RETURN(result.s_addr);
    return (result);
}
