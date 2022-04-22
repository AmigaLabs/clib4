/*
 * $Id: stdlib_set_errno.c,v 1.2 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__set_errno(int new_errno)
{
    ENTER();
    SHOWVALUE(new_errno);

	errno = new_errno;

    LEAVE();
}
