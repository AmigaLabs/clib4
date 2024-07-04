/*
 * $Id: unistd_alarm.c.c,v 1.0 2022-03-16 09:34:24 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

unsigned int
alarm(unsigned seconds) {
    /* Call __setitimer */
    struct itimerval it = {
            .it_value.tv_sec = seconds,
            .it_value.tv_usec = 0,
            .it_interval.tv_sec = 0,
            .it_interval.tv_usec = 0
    }, old = {0};
    __setitimer(-1, &it, &old);
    return old.it_value.tv_sec + !!old.it_value.tv_usec;
}
