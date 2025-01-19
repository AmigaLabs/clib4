/*
 * $Id: getpagesize.c,v 1.0 2022-12-28 12:04:22 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int getpagesize(void) {
    return (int) sysconf(_SC_PAGESIZE);
}
