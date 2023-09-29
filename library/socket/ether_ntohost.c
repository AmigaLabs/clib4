/*
 * $Id: socket_ether_ntohost.c,v 1.0 2023-06-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
ether_ntohost(char *hostname, const struct ether_addr *e) {
    struct ether_addr currente;
    char buf[512];
    char *val = buf;
    int ret = -1;
    FILE *f;

    if ((f = fopen(_PATH_ETHERS, "r")) == NULL) {
        return ret;
    }

    while (fscanf(f, "%[^\n] ", val) == 1) {
        if ((ether_line(val, &currente, hostname) == 0) &&
            (bcmp(e, &currente, sizeof(currente)) == 0)) {
            ret = 0;
            break;
        }
    }

    fclose(f);
    return ret;
}