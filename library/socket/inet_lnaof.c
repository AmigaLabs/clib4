/*
 * $Id: socket_inet_lnaof.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

unsigned long
inet_lnaof(struct in_addr in)
{
	unsigned long result;

	ENTER();

	assert(__SocketBase != NULL);

	result = __Inet_LnaOf(in.s_addr);

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
