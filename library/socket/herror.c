/*
 * $Id: socket_herror.c,v 1.0 2022-08-09 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void herror(const char *msg) {
    fprintf(stderr, "%s%s%s\n", msg ? msg : "", msg ? ": " : "", hstrerror(h_errno));
}
