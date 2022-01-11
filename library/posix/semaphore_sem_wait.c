/*
 * $Id: semaphore_sem_wait.c,v 1.0 2021-01-18 17:08:03 apalmate Exp $
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

int sem_wait(sem_t *sem)
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
        struct Node waitnode;
        uint32 signals;

        waitnode.ln_Pri = thistask->tc_Node.ln_Pri;
        waitnode.ln_Name = (STRPTR)thistask;

        Enqueue(&isem->waitlist, &waitnode);

        do
        {
            SetSignal(0, SIGF_SINGLE);
            ReleaseSemaphore(&isem->accesslock);
            signals = Wait(SIGBREAKF_CTRL_C | SIGF_SINGLE);
            ObtainSemaphore(&isem->accesslock);
        } while (isem->value == 0 && (signals & SIGBREAKF_CTRL_C) == 0);

        if (isem->value == 0)
        {
            Remove(&waitnode);
            ReleaseSemaphore(&isem->accesslock);
            __set_errno(EINTR);
            return -1;
        }
    }
    isem->value--;
    ReleaseSemaphore(&isem->accesslock);

    return 0;
}
