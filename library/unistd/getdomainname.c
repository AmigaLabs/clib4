/*
 * $Id: socket_getdomainname.c,v 1.0 2022-04-20 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
getdomainname(char *name, size_t len) {
    int result = ERROR;

    ENTER();

    SHOWPOINTER(name);
    SHOWVALUE(len);

    assert(name != NULL);
    assert(__SocketBase != NULL);

    if (name == NULL) {
        SHOWMSG("invalid name");

        __set_errno(EFAULT);
        goto out;
    }

    /* GetDefaultDomainName return TRUE if domain is set and FALSE if not set
     * so we have to negate it to avoid an error
     */
    result = !__getdomainname((STRPTR) name, len);

out:

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}