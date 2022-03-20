/*
 * $Id: socket_get_h_errno.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

/****************************************************************************/

int
__get_h_errno(void)
{
	return(h_errno);
}
