/*
 * $Id: stdlib_set_errno.c,v 1.4 2024-07-20 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__set_errno(int new_errno) {
    SHOWVALUE(new_errno);
    if (&errno != NULL)
	    errno = new_errno;
}

void
__set_errno_r(struct _clib4 *__clib4, int new_errno) {
    SHOWVALUE(new_errno);
    if (&errno_r != NULL)
        errno_r = new_errno;
}