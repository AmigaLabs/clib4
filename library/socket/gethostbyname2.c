/*
 * $Id: socket_gethostbyname2.c,v 1.0 2022-08-09 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyname2(const char *name, int af) {
    static struct hostent *h;
    size_t size = 63;
    struct hostent *res;
    int err;

    do {
        free(h);
        h = malloc(size += size + 1);
        if (!h) {
            h_errno = NO_RECOVERY;
            return 0;
        }
        err = gethostbyname2_r(name, af, h, (void *) (h + 1), size - sizeof *h, &res, &h_errno);
    } while (err == ERANGE);
    return err ? 0 : h;
}
