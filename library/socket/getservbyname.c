/*
 * $Id: socket_getservbyname.c,v 1.5 2022-08-09 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct servent *
getservbyname(const char *name, const char *proto) {
    static struct servent se;
    static char *buf[2];
    struct servent *res;
    if (getservbyname_r(name, proto, &se, (void *) buf, sizeof buf, &res))
        return 0;
    return &se;
}
