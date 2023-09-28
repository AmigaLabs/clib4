/*
 * $Id: socket_gethostbyname.c,v 1.5 2022-08-09 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyname(const char *name) {
    return gethostbyname2(name, AF_INET);
}
