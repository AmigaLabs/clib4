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
    if (getservbyport_r(port, proto, &se, (void *) buf, sizeof buf, &res)) {
        /* if getservbyport_r has no results try to fall back to Roadshow __getservbyport
         * because Roadshow doesn't create any file in DEVS:Internet that can be used
         * by clib4 if DHCP is enabled
         */
        if (proto == NULL) {
            __set_errno(EFAULT);
        }
        else
            return __getservbyport(port, (char *) proto);
    }
    return &se;
}
