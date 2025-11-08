/*
$Id: pthread_condattr_setclock.c,v 1.00 2025-10-31 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_condattr_setclock(pthread_condattr_t *attrp, clockid_t clk) {
	if (clk != CLOCK_REALTIME && clk != CLOCK_MONOTONIC)
		return (EINVAL);

	attrp->clock_type = clk;

	return 0;
}