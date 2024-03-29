/*
 * $Id: socket_getprotobyname.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct protoent *
getprotobyname(const char *name) {
    struct protoent *result = NULL;

    ENTER();

    assert(name != NULL);
    DECLARE_SOCKETBASE();

    if (name == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    result = __getprotobyname((char *) name);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
