/*
 * $Id: time_clock.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
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

static struct DateStamp start_time;

CLIB_CONSTRUCTOR(clock_init) {
    ENTER();

    /* Remember when this program was started. */
    DateStamp(&start_time);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}

clock_t
clock(void) {
    struct DateStamp now;
    LONG minutes_now, ticks_now;
    LONG minutes_start, ticks_start;
    clock_t result;

    ENTER();

    /* Get the current time. */
    DateStamp(&now);

    /* Break the current and start time down into minutes and ticks. */
    minutes_now = now.ds_Days * 24 * 60 + now.ds_Minute;
    ticks_now = now.ds_Tick;

    minutes_start = start_time.ds_Days * 24 * 60 + start_time.ds_Minute;
    ticks_start = start_time.ds_Tick;

    /* Subtract the start time from the current time. We start
     * with the ticks.
     */
    ticks_now -= ticks_start;

    /* Check for underflow. */
    while (ticks_now < 0) {
        /* Borrow a minute from the current time. */
        ticks_now += 60 * TICKS_PER_SECOND;

        minutes_now--;
    }

    /* Now for the minutes. */
    minutes_now -= minutes_start;

    /* Check if any time has passed at all, then return the difference. */
    if (minutes_now >= 0)
        result = (clock_t)(minutes_now * 60 * TICKS_PER_SECOND + ticks_now);
    else
        result = (clock_t) 0;

    RETURN(result);
    return (result);
}
