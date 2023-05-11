/*
 * $Id: resource_getrusage.c,v 1.0 2021-01-19 16:35:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/resource.h>

int
getrusage(int who, struct rusage *rusage) {
    ENTER();
    if (rusage == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }
    struct _clib2 *__clib2 = __CLIB2;

    SHOWVALUE(who);
    SHOWPOINTER(rusage);

    long clock[2];
    int status = 0;
    struct TimerIFace *ITimer = __CLIB2->__ITimer;

    if (__clib2 == NULL) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    switch (who) {
        case RUSAGE_SELF: {
                GetSysTime((struct TimeVal *) clock);
                clock[0] -= __clib2->clock.Seconds;
                clock[1] -= __clib2->clock.Microseconds;
                if (clock[1] < 0) {
                    clock[1] += 1000000;
                    clock[0]--;
                }

                memcpy(rusage, &__clib2->ru, sizeof(struct rusage));
                rusage->ru_utime.tv_sec = clock[0];
                rusage->ru_utime.tv_usec = clock[1];
            }
            break;

        case RUSAGE_CHILDREN:
            memcpy(rusage, &__clib2->ru, sizeof(struct rusage));
            break;

        default:
            __set_errno(EINVAL);
            status = -1;
            break;
    }

    RETURN(status);
    return status;
}