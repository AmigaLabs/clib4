/*
 * $Id: time_itimer_tasks.c,v 1.0 2022-03-15 08:39:45 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif /* PROTO_TIMER_H */

int itimer_real_task() {
    int *timertype;
    struct MsgPort *tmr_real_mp = NULL;
    struct TimeRequest *tmr_real_tr = NULL;
    ULONG wait_mask;
    BOOL Success = FALSE;

    struct Process *thisTask = (struct Process *)FindTask(NULL);
    timertype = (int *)thisTask->pr_Task.tc_UserData;
    //printf("timertype=%d\n", *timertype);

    /* Create itimer timers and message ports */
    tmr_real_mp = AllocSysObjectTags(ASOT_PORT,
                                     ASOPORT_Action, PA_SIGNAL,
                                     ASOPORT_AllocSig, FALSE,
                                     ASOPORT_Signal, SIGB_SINGLE,
                                     ASOPORT_Target, FindTask(NULL),
                                     TAG_DONE);
    if (!tmr_real_mp) {
        goto out;
    }
    tmr_real_tr = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_ReplyPort, tmr_real_mp, ASOIOR_Size, sizeof(struct TimeRequest), TAG_END);
    if (!tmr_real_tr) {
        goto out;
    }
    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)tmr_real_tr, 0) != OK)
    {
        goto out;
    }
    SetSignal(0, SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | (1UL << tmr_real_mp->mp_SigBit));

    tmr_real_tr->Request.io_Command = TR_ADDREQUEST;
    ObtainSemaphore(&__global_clib2->__tmr_access_sem);
    tmr_real_tr->Time.Seconds = __global_clib2->tmr_time->it_value.tv_sec;
    tmr_real_tr->Time.Microseconds = __global_clib2->tmr_time->it_value.tv_usec;
    ReleaseSemaphore(&__global_clib2->__tmr_access_sem);

    printf("seconds = %d\n", tmr_real_tr->Time.Seconds);
    printf("useconds = %d\n", tmr_real_tr->Time.Microseconds);
    /* Loop until timer expires and restart or we interrupt it */
    while (TRUE) {
        SendIO((struct IORequest *) tmr_real_tr);
        wait_mask = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | 1L << tmr_real_mp->mp_SigBit;
        uint32 signals = Wait(wait_mask);
        if (signals & SIGBREAKF_CTRL_E) {
            Printf("SIGBREAKF_CTRL_E\n");
            if (CheckIO((struct IORequest *) tmr_real_tr))  /* If request is complete... */
                WaitIO((struct IORequest *) tmr_real_tr);   /* clean up and remove reply */
            AbortIO((struct IORequest *) tmr_real_tr);
            /* Exit from while */
            break;
        }
        else if (signals & SIGBREAKF_CTRL_D) {
            Printf("SIGBREAKF_CTRL_D\n");
            /* This is used to reset the timer with new value without raising the signal */
            if (CheckIO((struct IORequest *) tmr_real_tr))  /* If request is complete... */
                WaitIO((struct IORequest *) tmr_real_tr);   /* clean up and remove reply */
            AbortIO((struct IORequest *) tmr_real_tr);

            ObtainSemaphore(&__global_clib2->__tmr_access_sem);
            tmr_real_tr->Time.Seconds = __global_clib2->tmr_time->it_value.tv_sec;
            tmr_real_tr->Time.Microseconds = __global_clib2->tmr_time->it_value.tv_usec;
            ReleaseSemaphore(&__global_clib2->__tmr_access_sem);

            Printf("SEconds = %ld\n", __global_clib2->tmr_time->it_value.tv_sec);
            Printf("USeconds = %ld\n", __global_clib2->tmr_time->it_value.tv_usec);

            Printf("Done\n");
        }
        else {
            Printf("mp_SigBit\n");
            WaitIO((struct IORequest *) tmr_real_tr);

            /* Raise SIGALARM signal so the handlers can execute the associated code */
            raise(SIGALRM);

            /* Set the start time of new request used to calculate values in getitimer */
            gettimeofday(&__global_clib2->tmr_start_time, NULL);

            /* Reset the timer adding seconds to timerequest */
            tmr_real_tr->Time.Seconds += __global_clib2->tmr_time->it_value.tv_sec;
            tmr_real_tr->Time.Microseconds += __global_clib2->tmr_time->it_value.tv_usec;
        }

        Printf("useconds = %ld\n", tmr_real_tr->Time.Microseconds);
        Printf("seconds = %ld\n", tmr_real_tr->Time.Seconds);
    }
    Printf("success\n");
    Success = TRUE;

out:
    /* Free itimer objects */
    if (tmr_real_mp) {
        FreeSysObject(ASOT_PORT, tmr_real_mp);
        tmr_real_mp = NULL;
    }
    if (tmr_real_tr) {
        if (tmr_real_tr->Request.io_Device != NULL)
            CloseDevice((struct IORequest *)tmr_real_tr);
        FreeSysObject(ASOT_IOREQUEST, tmr_real_tr);
        tmr_real_tr = NULL;
    }
    Printf("all stuff closed\n");

    if (Success)
        return RETURN_OK;
    else
        return RETURN_FAIL;
}
