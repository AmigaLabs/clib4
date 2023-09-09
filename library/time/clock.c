/*
 * $Id: time_clock.c,v 1.8 2022-07-18 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

CLIB_CONSTRUCTOR(clock_init) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    /* Remember when this program was started. */
    SHOWMSG("Calling gettimeofday");
    gettimeofday(&__clib2->clock, NULL);
    /* Generate random seed */
    __clib2->__random_seed = time(NULL);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}

clock_t
clock(void) {
    struct timeval now;
    uint64 usec_now, usec_start;
    clock_t result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    __check_abort();

    /* Get the current time. */
    gettimeofday(&now, NULL);

    usec_now = now.tv_sec * 1000000ULL + now.tv_usec;
    usec_start = __clib2->clock.tv_sec * 1000000ULL + __clib2->clock.tv_usec;
    /* Subtract the start time from the current time. */
    usec_now -= usec_start;

    result = (clock_t) (usec_now * CLK_TCK / 1000000);

    RETURN(result);
    return result;
}
