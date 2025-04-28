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
    struct MsgPort *messagePort;
    struct TimeRequest *timeRequest;

    __check_abort_f(__clib4);

    DECLARE_TIMEZONEBASE_R(__clib4);

    SHOWMSG("Obtaining __timer_semaphore");
    ObtainSemaphore(__clib4->__timer_semaphore);
    messagePort = AllocSysObjectTags(ASOT_PORT,
                                     ASOPORT_AllocSig, FALSE,
                                     ASOPORT_Signal,   SIGB_SINGLE,
                                     TAG_DONE);
    if (!messagePort)
        return ENOMEM;

    timeRequest = AllocSysObjectTags(ASOT_IOREQUEST,
                                     ASOIOR_Duplicate, __clib4->__timer_request,
                                     ASOIOR_Size, sizeof(struct TimeRequest),
                                     ASOIOR_ReplyPort, messagePort,
                                     TAG_END);
    if (!timeRequest) {
        FreeSysObject(ASOT_IOREQUEST, messagePort);
        return ENOMEM;
    }

    timeRequest->Request.io_Command = timercmd;
    timeRequest->Time.Seconds = tv->tv_sec;
    timeRequest->Time.Microseconds = tv->tv_usec;

    SHOWMSG("Send IO Request");
	SendIO((struct IORequest *) timeRequest);

    wait_mask = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_C | ( 1L << messagePort->mp_SigBit );
    /* Wait for signals */
    SHOWMSG("Waiting for signal");
    uint32 signals = Wait(wait_mask);
    if (signals & SIGBREAKF_CTRL_C || signals & SIGBREAKF_CTRL_E) {
        if (CheckIO((struct IORequest *) timeRequest))  /* If request is complete... */
            WaitIO((struct IORequest *) timeRequest);   /* clean up and remove reply */
        AbortIO((struct IORequest *) timeRequest);
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
    SHOWMSG("Wait IO");
    WaitIO((struct IORequest *) timeRequest);

    SHOWVALUE(timeRequest->Time.Seconds);
    SHOWVALUE(timeRequest->Time.Microseconds);
    tv->tv_sec = timeRequest->Time.Seconds;
    tv->tv_usec = timeRequest->Time.Microseconds;

    FreeSysObject(ASOT_IOREQUEST, timeRequest);
    FreeSysObject(ASOT_PORT, messagePort);

    ReleaseSemaphore(__clib4->__timer_semaphore);

    __check_abort_f(__clib4);

    RETURN(result);
    return result;
}
