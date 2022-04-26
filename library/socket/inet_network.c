/*
 * $Id: socket_inet_network.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

in_addr_t
inet_network(const char *cp) {
    in_addr_t result = ~0UL;

    ENTER();

    assert(cp != NULL);
    assert(__SocketBase != NULL);

    if (cp == NULL) {
        SHOWMSG("invalid cp parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = __inet_network((char *) cp);

out:

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
