/*
 * $Id: socket_setdomainname.c,v 1.0 2022-04-20 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "../socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
setdomainname(const char *name, size_t len) {
    int result = ERROR;

    ENTER();

    SHOWPOINTER(name);
    SHOWVALUE(len);

    assert(name != NULL);
    DECLARE_SOCKETBASE();

    if (name == NULL) {
        SHOWMSG("invalid name");

        __set_errno(EFAULT);
        goto out;
    }

    __setdomainname((STRPTR) name);

    if (strlen(name) > len) {
        name = NULL;
        __set_errno(EINVAL);
        goto out;
    }

    result = OK;

out:

    __check_abort();

    RETURN(result);
    return (result);
}