/*
 * $Id: socket_gethostbyname.c,v 1.6 2025-01-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyname(const char *name) {
    struct hostent *he = gethostbyname2(name, AF_INET);
    /* if he is NULL try to fall back to Roadshow __gethostbyname
     * because Roadshow doesn't create any file in DEVS:Internet that can be used
     * by clib4 if DHCP is enabled
     */
    if (he == NULL) {
        if (name == NULL) {
            __set_errno(EFAULT);
        }
        else
            he = __gethostbyname(name);
    }
    return he;
}
