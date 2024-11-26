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

    struct MsgPort *mp;
    struct TimeRequest *tr;
    ULONG wait_mask;
    int result = 0;
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    SHOWMSG("Clearing Signals");
    SetSignal(0, SIGB_SINGLE | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E);

    SHOWMSG("Allocating System Objects");
    mp = AllocSysObjectTags(ASOT_PORT,
                            ASOPORT_AllocSig, FALSE,
                            ASOPORT_Signal, SIGB_SINGLE,
                            TAG_DONE);
    if (!mp) {
        SHOWMSG("Cannot allocate Message Port");
        return ENOMEM;
    }

    tr = AllocSysObjectTags(ASOT_IOREQUEST,
                            ASOIOR_Duplicate, TimeReq,
                            ASOIOR_Size, sizeof(struct TimeRequest),
                            ASOIOR_ReplyPort, mp,
                            TAG_END);

    if (!tr) {
        SHOWMSG("Cannot allocate Timer Request");
        FreeSysObject(ASOT_PORT, mp);
        return ENOMEM;
    }

    tr->Request.io_Command = timercmd;
    tr->Time.Seconds = tv->tv_sec;
    tr->Time.Microseconds = tv->tv_usec;

    SHOWMSG("Send IO Request");
    SendIO((struct IORequest *) tr);
    wait_mask = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_C | 1L << mp->mp_SigBit;
    /* Wait for signals */
    SHOWMSG("Waiting for signal");
    uint32 signals = Wait(wait_mask);
    if (signals & SIGBREAKF_CTRL_C || signals & SIGBREAKF_CTRL_E) {
        if (CheckIO((struct IORequest *) tr))  /* If request is complete... */
            WaitIO((struct IORequest *) tr);   /* clean up and remove reply */
        AbortIO((struct IORequest *) tr);
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
    WaitIO((struct IORequest *) tr);

    SHOWVALUE("tr->Time.Seconds");
    SHOWVALUE("tr->Time.Microseconds");
    tv->tv_sec = tr->Time.Seconds;
    tv->tv_usec = tr->Time.Microseconds;

    SHOWMSG("Freeing Request Object");
    FreeSysObject(ASOT_IOREQUEST, tr);
    SHOWMSG("Freeing Message Port");
    FreeSysObject(ASOT_PORT, mp);

    __check_abort_f(__clib4);

    RETURN(result);
    return result;
}
