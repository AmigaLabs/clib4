/*
 * $Id: unistd_fdopen64.c,v 1.0 2021-02-05 13:28:00 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

FILE *
fdopen64(int file_descriptor, const char *type) {
    return fdopen(file_descriptor, type);
}