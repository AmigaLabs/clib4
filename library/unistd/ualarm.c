/*
 * $Id: unistd_ualarm.c.c,v 1.0 2022-03-16 09:34:24 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

useconds_t
ualarm(useconds_t value, useconds_t interval) {
    /* Call setitimer */
    struct itimerval it = {
            .it_interval.tv_usec = interval,
            .it_value.tv_usec = value
    }, it_old;
    setitimer(ITIMER_REAL, &it, &it_old);
    return it_old.it_value.tv_sec * 1000000 + it_old.it_value.tv_usec;
}
