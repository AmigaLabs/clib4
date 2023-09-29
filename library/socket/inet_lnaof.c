/*
 * $Id: socket_inet_lnaof.c,v 1.4 2022-08-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

in_addr_t
inet_lnaof(struct in_addr in) {
    in_addr_t result;

    ENTER();

    uint32_t h = in.s_addr;
    if (h>>24 < 128) return h & 0xffffff;
    if (h>>24 < 192) return h & 0xffff;
    result = h & 0xff;

    __check_abort();

    RETURN(result);
    return (result);
}
