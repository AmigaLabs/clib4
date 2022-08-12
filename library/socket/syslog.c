/*
 * $Id: socket_syslog.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void
syslog(int priority, const char *message, ...) {
    va_list ap;
    va_start(ap, message);
    vsyslog(priority, message, ap);
    va_end(ap);
}