/*
 * $Id: socket_getservbyport.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct servent *
getservbyport(int port, const char *proto)
{
	struct servent *result = NULL;

	ENTER();

	assert(proto != NULL);
	assert(__SocketBase != NULL);

    if (proto == NULL)
    {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

	result = __getservbyport(port, (char *)proto);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
