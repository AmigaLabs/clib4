/*
 * $Id: socket_gethostbyname_r.c,v 1.0 2022-08-09 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
gethostbyname_r(const char *name, struct hostent *h, char *buf, size_t buflen, struct hostent **res, int *err) {
    return gethostbyname2_r(name, AF_INET, h, buf, buflen, res, err);
}
