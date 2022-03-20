/*
 * $Id: times_times.c,v 1.0 2021-02-07 12:42:45 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

clock_t
times(struct tms *tp)
{
    struct rusage ru;
    time_t tim;

    if (getrusage(RUSAGE_SELF, &ru) < 0)
        return (clock_t)-1;

    tp->tms_utime = (clock_t)ru.ru_utime.tv_sec * CLK_TCK + (ru.ru_utime.tv_usec * CLK_TCK) / 1000000;
    tp->tms_stime = (clock_t)ru.ru_stime.tv_sec * CLK_TCK + (ru.ru_stime.tv_usec * CLK_TCK) / 1000000;

    if (getrusage(RUSAGE_CHILDREN, &ru) < 0)
        return (clock_t)-1;

    tp->tms_cutime = 0;
    tp->tms_cstime = 0;

    if ((tim = time(NULL)) == (time_t)-1)
        return (clock_t)-1;

    /* The value we are supposed to return does not fit in 32 bits.
      Still, it is useful if you are interested in time differences
      in CLK_TCKths of a second.
   */

    return (clock_t)tim * CLK_TCK;
}