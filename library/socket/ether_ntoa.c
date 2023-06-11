/*
 * $Id: socket_ether_ntoa.c,v 1.0 2023-06-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

char *
ether_ntoa(const struct ether_addr *p_a) {
    static char x[18];
    return ether_ntoa_r(p_a, x);
}
