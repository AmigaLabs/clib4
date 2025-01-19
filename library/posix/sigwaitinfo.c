/*
 * $Id: sigwaitinfo.c,v 1.0 2024-08-30 14:58:00 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigwaitinfo(const sigset_t *set, siginfo_t *info) {
	int ret;
	
	ENTER();

	SHOWPOINTER(set);
	SHOWPOINTER(info);

	ret = sigtimedwait (set, info, 0 );

	RETURN(ret);

	return ret;
}