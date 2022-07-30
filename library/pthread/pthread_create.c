/*
  $Id: pthread_create.c,v 1.00 2022-07-18 12:09:49 clib2devs Exp $

  Copyright (C) 2014 Szilard Biro
  Copyright (C) 2018 Harry Sintonen
  Copyright (C) 2019 Stefan "Bebbo" Franke - AmigaOS 3 port

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "common.h"
#include "pthread.h"

static uint32
StarterFunc() {
    int foundkey = TRUE;
    struct StackSwapStruct stack;
    volatile BOOL stackSwapped = FALSE;

    struct Process *startedTask = (struct Process *) FindTask(NULL);
    ThreadInfo *inf = (ThreadInfo *)startedTask->pr_EntryData;

    // custom stack requires special handling
    if (inf->attr.stackaddr != NULL && inf->attr.stacksize > 0) {
        stack.stk_Lower = inf->attr.stackaddr;
        stack.stk_Upper = (ULONG)((APTR) stack.stk_Lower) + inf->attr.stacksize;
        stack.stk_Pointer = (APTR) stack.stk_Upper;

        StackSwap(&stack);
        stackSwapped = TRUE;
    }

    // set a jump point for pthread_exit
    if (!setjmp(inf->jmp)) {
        Printf("[%s] Mark thread as RUNNING\n\n", inf->name);
        inf->status = THREAD_STATE_RUNNING;
        inf->ret = inf->start(inf->arg);
        Printf("\n[%s] Exit from start function in setjmp\n", inf->name);
    }
    else
        Printf("[%s] Exit from longjmp\n", inf->name);

    Printf("[%s] Calling pthread_cleanup_pop\n", inf->name);
    pthread_cleanup_pop(1);

    // destroy all non-NULL TLS key values
    // since the destructors can set the keys themselves, we have to do multiple iterations
    Printf("[%s] Destroy all non-NULL TLS key values\n", inf->name);
    ObtainSemaphoreShared(&tls_sem);
    for (int j = 0; foundkey && j < PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
        foundkey = FALSE;
        for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
            if (tlskeys[i].used && tlskeys[i].destructor && inf->tlsvalues[i]) {
                void *oldvalue = inf->tlsvalues[i];
                inf->tlsvalues[i] = NULL;
                tlskeys[i].destructor(oldvalue);
                foundkey = TRUE;
            }
        }
    }
    ReleaseSemaphore(&tls_sem);

    if (stackSwapped)
        StackSwap(&stack);

    Printf("[%s] Finishing stuff\n", inf->name);
    if (inf->status == THREAD_STATE_RUNNING) {
        if (!inf->detached) {
            Printf("[%s] Signal parent %p\n", inf->name, inf->parent);
            // tell the parent thread that we are done
            Forbid();
            Signal(inf->parent, SIGF_PARENT);
        } else {
            // no one is waiting for us, do the clean up
            Printf("[%s] ObtainSemaphore on StarterFunc\n", inf->name);
            ObtainSemaphore(&thread_sem);
            Printf("[%s] Got Semaphore on StarterFunc\n", inf->name);
            _pthread_clear_threadinfo(inf);
            Printf("[%s] Mark thread as IDLE\n", inf->name);
            ReleaseSemaphore(&thread_sem);
            Printf("[%s] ReleaseSemaphore on StarterFunc\n", inf->name);
        }
    }
    else
        Printf("[%s] Thread was not running\n", inf->name);
    Printf("[%s] Exit StarterFunc\n", inf->name);

    return RETURN_OK;
}

int
pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg) {
    ThreadInfo *inf;
    char name[NAMELEN] = {0};
    size_t oldlen;
    pthread_t threadnew;
    struct Task *thisTask = FindTask(NULL);

    if (thread == NULL || start == NULL)
        return EINVAL;

    Printf("Obtaining new thread semaphore\n");
    ObtainSemaphore(&thread_sem);
    Printf("Obtainined\n");

    // grab an empty thread slot
    threadnew = GetThreadId(NULL);
    if (threadnew == PTHREAD_THREADS_MAX) {
        ReleaseSemaphore(&thread_sem);
        return EAGAIN;
    }

    // prepare the ThreadInfo structure
    inf = GetThreadInfo(threadnew);
    _pthread_clear_threadinfo(inf);

    inf->start = start;
    inf->arg = arg;
    inf->parent = thisTask;
    if (attr)
        inf->attr = *attr;
    else
        pthread_attr_init(&inf->attr);
    NewMinList(&inf->cleanup);
    inf->cancelstate = PTHREAD_CANCEL_ENABLE;
    inf->canceltype = PTHREAD_CANCEL_DEFERRED;
    inf->detached = inf->attr.detachstate == PTHREAD_CREATE_DETACHED;

    /* Ceck minimum stack size */
    int minStack = PTHREAD_STACK_MIN;
    int currentStack = (uint32) thisTask->tc_SPUpper - (uint32) thisTask->tc_SPLower;
    if (currentStack > minStack)
        currentStack = minStack;

    if (inf->attr.stacksize < minStack)
        inf->attr.stacksize = minStack;

    // let's trick CreateNewProc into allocating a larger buffer for the name
    snprintf(name, sizeof(name), "pthread id #%d", threadnew);
    oldlen = strlen(name);
    memset(name + oldlen, ' ', sizeof(name) - oldlen - 1);
    name[sizeof(name) - 1] = '\0';
    strncpy(inf->name, name, NAMELEN);

    BPTR fileIn  = Open("CONSOLE:", MODE_OLDFILE);
    BPTR fileOut = Open("CONSOLE:", MODE_OLDFILE);

    // start the child thread
    inf->task = (struct Task *) CreateNewProcTags(
            NP_Entry,                StarterFunc,
            NP_EntryData,            inf,
            inf->attr.stacksize ? TAG_IGNORE : NP_StackSize, inf->attr.stacksize,
            NP_Input,			     fileIn,
            NP_CloseInput,		     TRUE,
            NP_Output,			     fileOut,
            NP_CloseOutput,		     TRUE,
            NP_Name,                 name,
            NP_Child,                TRUE,
            NP_Cli,				     TRUE,
            TAG_DONE);

    if (0 == inf->task) {
        inf->parent = NULL;
        Close(fileIn);
        Close(fileOut);
        ReleaseSemaphore(&thread_sem);
        return EAGAIN;
    }

    Printf("[%s] ReleaseSemaphore on pthread_create\n", inf->name);
    ReleaseSemaphore(&thread_sem);

    if (thread != NULL) {
        *thread = threadnew;
    }

    Printf("\n[%s] pthread_create done\n", inf->name);
    return OK;
}