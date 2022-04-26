/*
 * $Id: socket_gethostbyaddr.c,v 1.6 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyaddr(const void *addr, socklen_t len, int type) {
    struct hostent *result = NULL;

    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(type);

    assert(addr != NULL);
    assert(__SocketBase != NULL);

    if (addr == NULL) {
        SHOWMSG("invalid addr parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = __gethostbyaddr((STRPTR) addr, len, type);

out:

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
