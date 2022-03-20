/*
 * $Id: socket_inet_addr.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

unsigned long
inet_addr(const char *addr)
{
	unsigned long result = 0xFFFFFFFFUL;

	ENTER();

	SHOWSTRING(addr);

	assert(addr != NULL);
	assert(__SocketBase != NULL);

    if (addr == NULL)
    {
        SHOWMSG("invalid parameter");

        __set_errno(EFAULT);
        goto out;
    }

	result = __inet_addr((char *)addr);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
