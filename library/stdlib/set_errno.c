/*
 * $Id: stdlib_set_errno.c,v 1.3 2022-08-20 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__set_errno(int new_errno) {
    if (&errno != NULL)
	    errno = new_errno;
}
