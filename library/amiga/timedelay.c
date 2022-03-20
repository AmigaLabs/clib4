/*
 * $Id: amiga_timedelay.c,v 1.4 2006-04-05 06:43:56 clib2devs Exp $
*/

#include <devices/timer.h>
#include <clib/alib_protos.h>

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timeval		TimeVal
#define tv_secs		Seconds
#define tv_micro	Microseconds

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

LONG
TimeDelay(LONG unit, ULONG seconds, ULONG micros) {
    struct timeval tv;
    LONG error;

    tv.tv_secs = seconds;
    tv.tv_micro = micros;

    error = DoTimer(&tv, unit, TR_ADDREQUEST);

    return (error);
}
