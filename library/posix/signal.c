/*
 * $Id: signal_signal.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

/****************************************************************************/

void (*signal(int sig, void (* handler)(int)))(int)
{
	int table_entry = sig - SIGHUP;
	void (*result)(int) = SIG_ERR;

	ENTER();

	SHOWVALUE(sig);
	SHOWPOINTER(handler);

	if(sig < SIGHUP || sig > NSIG || handler == SIG_ERR)
	{
		SHOWMSG("unsupported signal");

		__set_errno(EINVAL);
		goto out;
	}

	result = (void (*)(int))__signal_handler_table[table_entry];

	__signal_handler_table[table_entry] = (signal_handler_t)handler;

 out:

	RETURN(result);
	return(result);
}
