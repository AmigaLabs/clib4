/*
 * $Id: socket_gethostid.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

long gethostid(void) {
    long result;

    ENTER();

    assert(__SocketBase != NULL);

    result = __gethostid();

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
