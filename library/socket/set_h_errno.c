/*
 * $Id: socket_set_h_errno.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
__set_h_errno(int new_h_errno) {
    if (&h_errno != NULL)
        h_errno = new_h_errno;
}
