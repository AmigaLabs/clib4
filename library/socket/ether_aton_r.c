/*
 * $Id: socket_ether_aton_r.c,v 1.0 2023-06-10 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct ether_addr *
ether_aton_r(const char *x, struct ether_addr *p_a) {
    struct ether_addr a;
    char *y;
    for (int ii = 0; ii < 6; ii++) {
        unsigned long int n;
        if (ii != 0) {
            if (x[0] != ':') return 0; /* bad format */
            else x++;
        }
        n = strtoul(x, &y, 16);
        x = y;
        if (n > 0xFF) return 0; /* bad byte */
        a.ether_addr_octet[ii] = n;
    }
    if (x[0] != 0) return 0; /* bad format */
    *p_a = a;
    return p_a;
}
