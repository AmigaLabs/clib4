/*
 * $Id: socket_wait_select.c,v 1.4 2008-04-16 07:53:40 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int 
waitselect(int num_fds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout, unsigned long *signal_mask)
{
	struct timeval other_timeout;
	int result;

	/* This is a workaround for a bug in the Roadshow TCP/IP stack which has been
	   fixed long ago: if a signal is received, as given in the 'signal_mask'
	   parameter, the WaitSelect() function may not detect it if the timeout
	   is zero. */
	if (signal_mask != NULL && timeout != NULL && timeout->tv_secs == 0 && timeout->tv_micro == 0)
	{
		/* Substitute a ten microsecond timeout. */
		other_timeout.tv_secs = 0;
		other_timeout.tv_micro = 10000;

		timeout = &other_timeout;
	}

	result = __select(num_fds, read_fds, write_fds, except_fds, timeout, signal_mask);

	return (result);
}
