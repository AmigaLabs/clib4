/*
 * $Id: unistd_sleep.c,v 1.6 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

unsigned int
sleep(unsigned int seconds) {
    ENTER();

    struct _clib4 *__clib4 = __CLIB4;
    int microseconds = seconds * 1000000;
    unsigned int result;

    SHOWVALUE(seconds);
    SHOWVALUE(microseconds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microseconds;

    __clib4->__timer_busy = TRUE;
    result = __time_delay(TR_ADDREQUEST, &tv); // EINTR can be returned inside the call
    __clib4->__timer_busy = FALSE;
    if (result == EINTR) {
        /* If a timer has been interrupted by a SIGALRM do we have to exit like on Linux?
         * At moment EINTR is returned only if we got an alarm() signal
         */
        _exit(RETURN_ERROR);
    }

    RETURN(result);
    return (result);
}
