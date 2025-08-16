/*
 * $Id: time_clock.c,v 1.8 2022-07-18 12:04:27 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    /* Remember when this program was started. */
    SHOWMSG("Calling gettimeofday");
    gettimeofday(&__clib4->clock, NULL);
    /* Generate random seed */
    __clib4->__random_seed = time(NULL);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}

clock_t
clock(void) {
    struct tms tim_s;
    clock_t res;

    if ((res = times(&tim_s)) != -1U)
        res = (tim_s.tms_utime + tim_s.tms_stime + tim_s.tms_cutime + tim_s.tms_cstime);

    return res;
}
