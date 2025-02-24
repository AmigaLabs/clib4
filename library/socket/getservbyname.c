/*
 * $Id: socket_getservbyname.c,v 1.6 2025-01-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct servent *
getservbyname(const char *name, const char *proto) {
    static struct servent se;
    static char *buf[2];
    struct servent *res;
    if (getservbyname_r(name, proto, &se, (void *) buf, sizeof buf, &res)) {
        /* if getservbyname_r has no results try to fall back to Roadshow __getservbyname
         * because Roadshow doesn't create any file in DEVS:Internet that can be used
         * by clib4 if DHCP is enabled
         */
        if (name == NULL || proto == NULL) {
            __set_errno(EFAULT);
        }
        else
            return __getservbyname((char *) name, (char *) proto);
    }
    return &se;
}
