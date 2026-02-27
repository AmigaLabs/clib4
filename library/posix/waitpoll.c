/*
 * $Id: waitpoll.c,v 1.0 2024-06-25 11:50:0 clib4devs Exp $
*/

#ifndef  _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
waitpoll(struct pollfd *fds, nfds_t nfds, int timeout, uint32_t *signals) {
    return __poll(fds, nfds, timeout, signals);
}