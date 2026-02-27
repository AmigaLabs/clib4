/*
 * $Id: posix_sigwait.,v 1.0 2024-08-18 12:04:24 clib4devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

int
sigwait(const sigset_t *set, int *sig) {
	siginfo_t si;
	int ret;

	ENTER();

	SHOWPOINTER(set);

	do
    	ret = sigtimedwait (set, &si, 0);
  		/* Applications do not expect sigwait to return with EINTR, and the
		   error code is not specified by POSIX.  */
	while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		RETURN(errno);

		return errno;
	}

	*sig = si.si_signo;

	SHOWVALUE(*sig);

	RETURN(0);

	return 0;	
}