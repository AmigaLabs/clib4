/*
 * $Id: unistd_usleep.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
usleep(unsigned long microseconds) {
    int result;
	struct _clib4 *__clib4 = __CLIB4;
    ENTER();

    SHOWVALUE(microseconds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microseconds;

    result = __time_delay(TR_ADDREQUEST, &tv); // EINTR can be returned inside the call

	if (result != OK) {
		__set_errno_r(__clib4, result);
		result = -1;
	}

    __check_abort_f(__clib4);

    RETURN(result);
    return result;
}
