/*
 * $Id: socket_getservbyname.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct servent *
getservbyname(const char *name, const char *proto)
{
	struct servent *result = NULL;

	ENTER();

	assert(name != NULL && proto != NULL);
	assert(__SocketBase != NULL);

    if (name == NULL || proto == NULL)
    {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

	result = __getservbyname((char *)name, (char *)proto);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
