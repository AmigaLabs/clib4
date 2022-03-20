/*
 * $Id: socket_inet_netof.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

in_addr_t
inet_netof(struct in_addr in)
{
	in_addr_t result;

	ENTER();

	assert(__SocketBase != NULL);

	result = __Inet_NetOf(in.s_addr);

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
