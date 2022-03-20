/*
 * $Id: unistd_time_delay.c,v 1.9 2006-04-05 08:39:45 clib2devs Exp $
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

int
__time_delay(ULONG timercmd, struct timeval *tv) {
    ENTER();

    struct MsgPort *mp;
    struct TimeRequest *tr;
    ULONG wait_mask;
    int result = 0;

    if (__check_abort_enabled)
        __check_abort();

    if (__timer_request == NULL)
        return EINVAL;

    SetSignal(0, SIGB_SINGLE | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E);

    mp = AllocSysObjectTags(ASOT_PORT,
                            ASOPORT_AllocSig, FALSE,
                            ASOPORT_Signal, SIGB_SINGLE,
                            TAG_DONE);

    tr = AllocSysObjectTags(ASOT_IOREQUEST,
                            ASOIOR_Duplicate, __timer_request,
                            ASOIOR_ReplyPort, mp,
                            ASOIOR_Size, sizeof(struct TimeRequest),
                            TAG_END);
    if (!tr) {
        FreeSysObject(ASOT_IOREQUEST, mp);
        return ENOMEM;
    }

    tr->Request.io_Command = timercmd;
    tr->Time.Seconds = tv->tv_sec;
    tr->Time.Microseconds = tv->tv_usec;

    SendIO((struct IORequest *) tr);
    wait_mask = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_C | 1L << mp->mp_SigBit;
    /* Wait for signals */
    uint32 signals = Wait(wait_mask);
    if (signals & SIGBREAKF_CTRL_C || signals & SIGBREAKF_CTRL_E) {
        if (CheckIO((struct IORequest *) tr))  /* If request is complete... */
            WaitIO((struct IORequest *) tr);   /* clean up and remove reply */
        AbortIO((struct IORequest *) tr);
        /* Return EINTR since the request has been interrupted */
        result = EINTR;
    }
    WaitIO((struct IORequest *) tr);

    tv->tv_sec = tr->Time.Seconds;
    tv->tv_usec = tr->Time.Microseconds;

    FreeSysObject(ASOT_MESSAGE, tr);
    FreeSysObject(ASOT_PORT, mp);

    RETURN(result);
    return result;
}
