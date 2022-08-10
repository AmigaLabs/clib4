/*
 * $Id: socket_inet_ntoa.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

char *
inet_ntoa(struct in_addr in) {
    char *result;

    ENTER();

    SHOWVALUE(in.s_addr);

    assert(__SocketBase != NULL);

    result = __Inet_NtoA(in.s_addr);

    SHOWSTRING(result);

    __check_abort();

    RETURN(result);
    return (result);
}
