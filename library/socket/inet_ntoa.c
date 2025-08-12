/*
 * $Id: socket_inet_ntoa.c,v 1.4 2022-08-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

char *
inet_ntoa(struct in_addr in) {
    char *result;

    ENTER();

    SHOWVALUE(in.s_addr);

    static char buf[16] = {0};
    unsigned char *a = (void *)&in;
    snprintf(buf, sizeof buf, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
    result = buf;

    SHOWSTRING(result);

    RETURN(result);
    return (result);
}
