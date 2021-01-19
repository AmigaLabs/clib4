/*
 * $Id: semaphore_sem_timedwait.c,v 1.0 2021-01-18 17:08:03 apalmate Exp $
 *
 * :ts=4
 *
 * Copyright (C) 2020-2021 Fredrik Wikstrom <fredrik@a500.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
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

#include "semaphore_private.h"

static struct TimeRequest *__open_timer(void)
{
    struct MsgPort *mp;
    struct TimeRequest *tr;

    mp = (struct MsgPort *)AllocSysObjectTags(ASOT_PORT, ASOPORT_Signal, SIGBREAKB_CTRL_C, TAG_END);
    if (mp != NULL)
    {
        tr = (struct TimeRequest *)AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_ReplyPort, mp, ASOIOR_Size, sizeof(*tr), TAG_END);
        if (tr != NULL)
        {
            if (OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)tr, 0) == IOERR_SUCCESS)
                return tr;

            FreeSysObject(ASOT_IOREQUEST, tr);
        }

        FreeSysObject(ASOT_PORT, mp);
    }

    return NULL;
}

static void __close_timer(struct TimeRequest *tr)
{
    struct MsgPort *mp = tr->Request.io_Message.mn_ReplyPort;

    CloseDevice((struct IORequest *)tr);

    FreeSysObject(ASOT_IOREQUEST, tr);
    FreeSysObject(ASOT_PORT, mp);
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    isem_t *isem = (isem_t *)*sem;
    if (isem == NULL)
    {
        __set_errno(EINVAL);
        return -1;
    }

    ObtainSemaphore(&isem->accesslock);
    if (isem->value == 0)
    {
        struct Task *thistask = FindTask(NULL);
        struct TimeRequest *tr;
        struct timeval tv;
        int64 timediff;
        struct Node waitnode;
        uint32 signals;

        tr = __open_timer();
        if (tr == NULL)
        {
            ReleaseSemaphore(&isem->accesslock);
            __set_errno(ENOMEM);
            return -1;
        }

        gettimeofday(&tv, NULL);
        timediff = ((int64)abs_timeout->tv_sec * 1000000000LL + abs_timeout->tv_nsec) -
                   ((int64)tv.tv_sec * 1000000000LL + tv.tv_usec * 1000);
        if (timediff <= 0)
        {
            ReleaseSemaphore(&isem->accesslock);
            __close_timer(tr);
            __set_errno(ETIMEDOUT);
            return -1;
        }

        waitnode.ln_Pri = thistask->tc_Node.ln_Pri;
        waitnode.ln_Name = (STRPTR)thistask;

        SetSignal(0, SIGF_SINGLE);
        Enqueue(&isem->waitlist, &waitnode);

        tr->Request.io_Command = TR_ADDREQUEST;
        tr->Time.Seconds = timediff / 1000000000LL;
        tr->Time.Microseconds = (timediff % 1000000000LL) / 1000;

        SendIO((struct IORequest *)tr);

        do
        {
            ReleaseSemaphore(&isem->accesslock);
            signals = Wait(SIGBREAKF_CTRL_C | SIGF_SINGLE);
            ObtainSemaphore(&isem->accesslock);
        } while (isem->value == 0 && (signals & SIGBREAKF_CTRL_C) == 0);

        if (isem->value == 0)
        {
            Remove(&waitnode);
            ReleaseSemaphore(&isem->accesslock);

            if (CheckIO((struct IORequest *)tr))
            {
                __set_errno(ETIMEDOUT);
            }
            else
            {
                AbortIO((struct IORequest *)tr);
                __set_errno(EINTR);
            }
            WaitIO((struct IORequest *)tr);

            __close_timer(tr);
            return -1;
        }

        __close_timer(tr);
    }
    isem->value--;
    ReleaseSemaphore(&isem->accesslock);

    return 0;
}
