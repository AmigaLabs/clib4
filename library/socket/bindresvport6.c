/*
 * $Id: socket_bindresvport6.c,v 1.0 2022-04-14 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
bindresvport6(int sd, struct sockaddr_in6 *sa) {
    return bindresvport(sd, (struct sockaddr_in *) sa);
}