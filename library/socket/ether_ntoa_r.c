/*
 * $Id: socket_ether_ntoa_r.c,v 1.0 2023-06-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

char *
ether_ntoa_r(const struct ether_addr *p_a, char *x) {
    char *y;
    y = x;
    for (int ii = 0; ii < 6; ii++) {
        x += sprintf(x, ii == 0 ? "%.2X" : ":%.2X", p_a->ether_addr_octet[ii]);
    }
    return y;
}