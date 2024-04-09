/*
 * $Id: time_clock.c,v 1.8 2024-04-09 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

void _clock_init(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* Remember when this program was started. */
    SHOWMSG("Calling gettimeofday");
    gettimeofday(&__clib4->clock, NULL);
    /* Generate random seed */
    __clib4->__random_seed = time(NULL);

    LEAVE();
}

clock_t
clock(void) {
    struct timeval now;
    uint64 usec_now, usec_start;
    clock_t result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __check_abort();

    /* Get the current time. */
    gettimeofday(&now, NULL);

    usec_now = now.tv_sec * 1000000ULL + now.tv_usec;
    usec_start = __clib4->clock.tv_sec * 1000000ULL + __clib4->clock.tv_usec;
    /* Subtract the start time from the current time. */
    usec_now -= usec_start;

    result = (clock_t) (usec_now * CLK_TCK / 1000000);

    RETURN(result);
    return result;
}
