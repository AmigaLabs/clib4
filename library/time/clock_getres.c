/*
 * $Id: time_getres.c,v 1.0 2020-01-13 17:27:27 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <time.h>

/* Get resolution of clock.  */
int clock_getres(clockid_t clock_id, struct timespec *res) {
    ENTER();

    int result = -1;

    switch (clock_id) {
        case CLOCK_MONOTONIC:
        case CLOCK_REALTIME: {
            /* This implementation assumes that the realtime clock has a
               resolution higher than 1 second.  This is the case for any
               reasonable implementation.  */
            res->tv_sec = 0;
            res->tv_nsec = 1000000000 / 1000;

            result = 0;
        }
        break;

        default:
            __set_errno(EINVAL);
            break;
    }

    RETURN(result);
    return result;
}
