/*
 * $Id: socket_getprotobynumber.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct protoent *
getprotobynumber(int proto) {
    struct protoent *result;

    ENTER();

    DECLARE_SOCKETBASE();

    result = __getprotobynumber(proto);

    __check_abort();

    RETURN(result);
    return (result);
}
