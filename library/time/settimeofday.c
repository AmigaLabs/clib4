/*
 * $Id: time_settimeofday.c,v 1.0 2022-02-24 18:31:57 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
settimeofday(const struct timeval *t, const struct timezone *tz) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    int result = -1;
    int32 __gmtoffset = 0;

    if (__clib4->__timer_busy) {
        __set_errno_r(__clib4, EPERM);
        RETURN(result);
        return result;
    }

    __clib4->__timer_busy = TRUE;

    __clib4->__timer_request->Request.io_Message.mn_ReplyPort = __clib4->__timer_port;
    __clib4->__timer_request->Request.io_Command = TR_SETSYSTIME;
    if (tz != NULL) {
        __gmtoffset = tz->tz_minuteswest;
    }
    /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
    __clib4->__timer_request->Time.Seconds = t->tv_sec - ((2922 * 24 * 60 + __gmtoffset) * 60);
    __clib4->__timer_request->Time.Microseconds = t->tv_usec;

    DoIO((struct IORequest *) __clib4->__timer_request);
    GetMsg(__clib4->__timer_port);

    result = 0;
    __set_errno_r(__clib4, 0);

    __clib4->__timer_busy = FALSE;

    RETURN(result);
    return result;
}