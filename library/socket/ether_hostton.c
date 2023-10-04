/*
 * $Id: socket_ether_hostton.c,v 1.0 2023-06-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
ether_hostton(const char *hostname, struct ether_addr *e) {
    char currenthost[256];
    char buf[512];
    char *val = buf;
    int ret = -1;
    FILE *f;

    if ((f = fopen(_PATH_ETHERS, "r")) == NULL) {
        return ret;
    }

    while (fscanf(f, "%[^\n] ", val) == 1) {
        if ((ether_line(val, e, currenthost) == 0) &&
            (strcmp(currenthost, hostname) == 0)) {
            ret = 0;
            break;
        }
    }

    fclose(f);
    return ret;
}
