/*
 * $Id: socket_gethostid.c,v 1.3 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

long
gethostid() {
    long result;

    ENTER();

    DECLARE_SOCKETBASE();

    result = __gethostid();

    __check_abort();

    RETURN(result);
    return (result);
}
