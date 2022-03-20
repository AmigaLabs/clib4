/*
 * $Id: socket_gethostbyname.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

struct hostent *
gethostbyname(const char *name)
{
	struct hostent *result = NULL;

	ENTER();

	SHOWSTRING(name);

	assert(name != NULL);
	assert(__SocketBase != NULL);

    if (name == NULL)
    {
        SHOWMSG("invalid name parameter");

        __set_errno(EFAULT);
        goto out;
    }

	result = __gethostbyname((STRPTR)name);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
