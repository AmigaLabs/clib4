/*
 * $Id: unistd_usleep.c,v 1.4 2021-02-02 18:49:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
nanosleep(const struct timespec *req, struct timespec *rem) {
    struct timeval tv;
    (void) (rem);
	struct _clib4 *__clib4 = __CLIB4;

    ENTER();
	if (req == NULL) {
		__set_errno_r(__clib4, EFAULT);
		RETURN(-1);
		return -1;
	}

	if (req->tv_sec < 0 || req->tv_nsec < 0 || req->tv_nsec >= 1000000000) {
		__set_errno_r(__clib4, EINVAL);
		RETURN(-1);
		return -1;
	}

    TIMESPEC_TO_TIMEVAL(&tv, req);

    int result = __time_delay(TR_ADDREQUEST, &tv); // EINTR can be returned inside the call
	/* nanosleep result is expecting -1 on error with errno() set */
	if (result != OK) {
		__set_errno_r(__clib4, result);
		result = -1;
	}

    __check_abort_f(__clib4);

    RETURN(result);
    return result;
}
