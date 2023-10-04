/*
 * $Id: socket_getservbyport.c,v 1.5 2022-08-09 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct servent *
getservbyport(int port, const char *proto) {
    static struct servent se;
    static long buf[32 / sizeof(long)];
    struct servent *res;
    if (getservbyport_r(port, proto, &se, (void *) buf, sizeof buf, &res))
        return 0;
    return &se;
}
