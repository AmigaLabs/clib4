/*
 * $Id: socket_set_h_errno.c,v 1.2 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
__set_h_errno(int new_h_errno) {
    if (&h_errno != NULL)
        h_errno = new_h_errno;
}

void
__set_h_errno_r(struct _clib4 *__clib4, int new_h_errno) {
    if (&h_errno_r != NULL)
        h_errno_r = new_h_errno;
}