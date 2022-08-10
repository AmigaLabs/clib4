/*
 * $Id: socket_getnetbyaddr.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct netent *
getnetbyaddr(in_addr_t net, int type) {
    struct netent *result = NULL;

    ENTER();

    assert(__SocketBase != NULL);

    result = __getnetbyaddr((ULONG) net, type);

    __check_abort();

    RETURN(result);
    return (result);
}
