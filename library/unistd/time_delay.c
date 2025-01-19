/*
 * $Id: unistd_time_delay.c,v 1.9 2006-04-05 08:39:45 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif /* PROTO_TIMER_H */

/* amiga_dotimer.c is a different implementation and use a different timer.device
 * we can't use the same implementation because it pass a Unit parameter.
 * However SIGBREAKF_CTRL_C handling should be implemented in the same way
 */

extern struct TimeRequest *TimeReq;

int
__time_delay(ULONG timercmd, struct timeval *tv) {
    ENTER();
	ULONG wait_mask;
    int result = 0;
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

	DECLARE_TIMEZONEBASE();

    ObtainSemaphore(__clib4->__timer_semaphore);

	__clib4->__timer_request->Request.io_Message.mn_ReplyPort = __clib4->__timer_port;
    __clib4->__timer_request->Request.io_Command = timercmd;
    __clib4->__timer_request->Time.Seconds = tv->tv_sec;
    __clib4->__timer_request->Time.Microseconds = tv->tv_usec;

    SHOWMSG("Send IO Request");
	SendIO((struct IORequest *) __clib4->__timer_request);
	
    wait_mask = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_C | ( 1L << __clib4->__timer_port->mp_SigBit );
    /* Wait for signals */
    SHOWMSG("Waiting for signal");
    uint32 signals = Wait(wait_mask);
    if (signals & SIGBREAKF_CTRL_C || signals & SIGBREAKF_CTRL_E) {
        if (CheckIO((struct IORequest *) __clib4->__timer_request))  /* If request is complete... */
            WaitIO((struct IORequest *) __clib4->__timer_request);   /* clean up and remove reply */
        AbortIO((struct IORequest *) __clib4->__timer_request);
        if (signals & SIGBREAKF_CTRL_E) {
            SHOWMSG("Received SIGBREAKF_CTRL_E");
            /* Return EINTR since the request has been interrupted by alarm */
            result = EINTR;
        } else {
            SHOWMSG("Received SIGBREAKF_CTRL_C. Reset it to set state");
            /* Reset SIGBREAKF_CTRL_C to set state since __check_abort can
             * break the execution
             */
            SetSignal(SIGBREAKF_CTRL_C, SIGBREAKF_CTRL_C);
        }
    }
    WaitIO((struct IORequest *) __clib4->__timer_request);

    SHOWVALUE(__clib4->__timer_request->Time.Seconds);
    SHOWVALUE(__clib4->__timer_request->Time.Microseconds);
    tv->tv_sec = __clib4->__timer_request->Time.Seconds;
    tv->tv_usec = __clib4->__timer_request->Time.Microseconds;

    __check_abort_f(__clib4);

 	ReleaseSemaphore(__clib4->__timer_semaphore);

    RETURN(result);
    return result;
}
