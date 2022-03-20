/*
 * $Id: stdlib_get_errno.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

int
__get_errno(void)
{
	return(errno);
}
