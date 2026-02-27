/*
 * $Id: resource_getrusage.c,v 1.0 2021-01-19 16:35:27 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    if (rusage == NULL) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(-1);
        return -1;
    }

    SHOWVALUE(who);
    SHOWPOINTER(rusage);

    long clock[2];
    int status = 0;
    struct TimerIFace *ITimer = __clib4->__ITimer;

    if (__clib4 == NULL) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(-1);
        return -1;
    }

    switch (who) {
        case RUSAGE_SELF: {
                GetSysTime((struct TimeVal *) clock);
                clock[0] -= __clib4->clock.tv_sec;
                clock[1] -= __clib4->clock.tv_usec;
                if (clock[1] < 0) {
                    clock[1] += 1000000;
                    clock[0]--;
                }

                memcpy(rusage, &__clib4->ru, sizeof(struct rusage));
                rusage->ru_utime.tv_sec = clock[0];
                rusage->ru_utime.tv_usec = clock[1];
            }
            break;

        case RUSAGE_CHILDREN:
            memcpy(rusage, &__clib4->ru, sizeof(struct rusage));
            break;

        default:
            __set_errno_r(__clib4, EINVAL);
            status = -1;
            break;
    }

    RETURN(status);
    return status;
}