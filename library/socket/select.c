/*
 * $Id: socket_select.c,v 1.16 2006-04-05 07:53:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
select(int num_fds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout) {
    int result;

    result = __select(num_fds, read_fds, write_fds, except_fds, timeout, NULL);

    return result;
}
