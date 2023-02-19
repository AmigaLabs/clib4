/*
 * $Id: socket_inet_addr.c,v 1.5 2022-09-16 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

unsigned long
inet_addr(const char *addr) {
    unsigned long result = 0xFFFFFFFFUL;

    ENTER();

    SHOWSTRING(addr);

    assert(addr != NULL);

    if (addr == NULL) {
        SHOWMSG("invalid parameter");

        __set_errno(EFAULT);
        goto out;
    }

    struct in_addr a;
    if (!inet_aton(addr, &a)) {
        __set_errno(EFAULT);
        goto out;
    }
    result = a.s_addr;

out:

    __check_abort();

    RETURN(result);
    return (result);
}
