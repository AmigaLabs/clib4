/*
 * $Id: socket_gethostbyaddr.c,v 1.7 2022-08-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyaddr(const void *addr, socklen_t len, int type) {
    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(type);

    assert(addr != NULL);

    int err = 1;

    if (addr == NULL) {
        SHOWMSG("invalid addr parameter");

        __set_errno(EFAULT);
        goto out;
    }

    static struct hostent *h;
    size_t size = 63;
    struct hostent *res;
    do {
        free(h);
        h = malloc(size += size + 1);
        if (!h) {
            h_errno = NO_RECOVERY;
            return 0;
        }
        err = gethostbyaddr_r(addr, len, type, h, (void *) (h + 1), size - sizeof *h, &res, &h_errno);
    } while (err == ERANGE);

out:

    __check_abort();

    RETURN(err ? 0 : 1);
    return err ? 0 : h;
}
