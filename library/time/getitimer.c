/*
 * $Id: time_getitimer.c,v 1.0 2022-03-14 18:06:24 clib4devs Exp $
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

static int
timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int
getitimer(int which, struct itimerval *curr_value) {
    struct _clib4 *__clib4 = __CLIB4;

    if (which < ITIMER_REAL || which > ITIMER_PROF) {
        __set_errno_r(__clib4, EINVAL);
        return -1;
    }

    if (curr_value == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return -1;
    }

    switch (which) {
        case ITIMER_REAL: {
            struct timeval tv, result;
            /* Get current time of day */
            gettimeofday(&tv, NULL);
            timeval_subtract(&result, &tv, &__clib4->tmr_start_time);
            curr_value->it_value.tv_sec = result.tv_sec;
            curr_value->it_value.tv_usec = result.tv_usec;
            curr_value->it_interval.tv_sec = __clib4->tmr_time.it_interval.tv_sec;
            curr_value->it_interval.tv_usec = __clib4->tmr_time.it_interval.tv_usec;
        }
            break;
        case ITIMER_VIRTUAL:
            __set_errno_r(__clib4, ENOSYS);
            return -1;
            break;
        case ITIMER_PROF:
            __set_errno_r(__clib4, ENOSYS);
            return -1;
            break;
        default:
            break;
    }
    return 0;
}