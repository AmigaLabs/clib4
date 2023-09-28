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

    ENTER();

    TIMESPEC_TO_TIMEVAL(&tv, req);

    int result = __time_delay(TR_ADDREQUEST, &tv); // EINTR can be returned inside the call
    if (result == EINTR) {
        /* If a timer has been interrupted by a SIGALRM do we have to exit like on Linux? */
        _exit(RETURN_ERROR);
    }

    RETURN(result);
    return result;
}
