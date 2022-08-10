/*
 * $Id: unistd_sleep.c,v 1.6 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

unsigned int
sleep(unsigned int seconds) {
    unsigned int result;

    ENTER();

    SHOWVALUE(seconds);

    int microseconds = seconds * 1000000;
    SHOWVALUE(microseconds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microseconds;

    result = __time_delay(TR_ADDREQUEST, &tv); // EINTR can be returned inside the call
    if (result == EINTR) {
        /* If a timer has been interrupted by a SIGALRM do we have to exit like on Linux?
         * At moment EINTR is returned only if we got an alarm() signal
         */
        _exit(RETURN_ERROR);
    }

    RETURN(result);
    return (result);
}
