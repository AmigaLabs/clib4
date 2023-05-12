/*
 * $Id: socket_gethostname.c,v 1.6 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
gethostname(const char *hostname, size_t size) {
    int result = ERROR;

    ENTER();

    SHOWPOINTER(hostname);
    SHOWVALUE(size);

    assert(hostname != NULL);
    DECLARE_SOCKETBASE();

    if (hostname == NULL) {
        SHOWMSG("invalid host name parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = __gethostname((STRPTR) hostname, size);

out:

    __check_abort();

    RETURN(result);
    return (result);
}
