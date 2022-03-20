/*
 * $Id: socket_freeaddrinfo.c,v 1.0 2021-01-13 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

void 
freeaddrinfo(struct addrinfo *ai)
{
	while (ai)
	{
		struct addrinfo *cur;

		cur = ai;
		ai = ai->ai_next;

		if (cur->ai_canonname)
			free(cur->ai_canonname);
		free(cur);
	}
}
