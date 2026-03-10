/*
  $Id: pthread_create.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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

extern struct DOSIFace *_IDOS;

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

// This is duplicate of killitimer() present in clib4 but is needed since it isn't exposed into interface
static void killitimer_by_thread(uint32 threadID) {
    struct _clib4 *__clib4 = __CLIB4;
    struct TimerNode *node, *next;
    
    /* Scan the timer list for timers belonging to the specified thread */
    for (node = (struct TimerNode *)__clib4->tmr_real_list.mlh_Head;
         (next = (struct TimerNode *)node->tn_Node.mln_Succ) != NULL;
         node = next) {
        
        if (node->tn_ThreadID == threadID) {
            struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
            int32 pid, process;
            
            pid = node->tn_Process->pr_ProcessID;
            /* Scan for process */
            process = ProcessScan(&h, (CONST_APTR) pid, 0);
            D(("Scan for process %ld (%ld) of thread %lu..\n", process, pid, threadID));
            
            while (process > 0) {
                D(("Waiting for process %ld to close..\n", pid));
                /* Send a SIGBREAKF_CTRL_F signal until the timer task return in Wait and can get the signal */
                Signal((struct Task *)node->tn_Process, SIGBREAKF_CTRL_F);
                process = ProcessScan(&h, (CONST_APTR) pid, 0);
                Delay(10);
            }
            
            SHOWMSG("Process closed.. Wait For Child\n");
            WaitForChildExit(pid);
            SHOWMSG("Done\n");
            
            /* Remove from list and free */
            Remove((struct Node *)&node->tn_Node);
            FreeVec(node);
			node = NULL;
        }
    }
}

static uint32
StarterFunc() {
    volatile int keyFound = TRUE;
    struct StackSwapStruct stack;
    volatile BOOL stackSwapped = FALSE;
	DECLARE_UTILITYBASE();

    struct Process *startedTask = (struct Process *) FindTask(NULL);
    ThreadInfo *inf = (ThreadInfo *) startedTask->pr_Task.tc_UserData;
	struct newThreadMessage *newThreadMessage = (struct newThreadMessage *) startedTask->pr_EntryData;

    D(("StarterFunc: thread %s STARTING (task=%p inf=%p)\n", inf->name, startedTask, inf));

    // set task TLS register
    set_tls_register(inf);

    struct _clib4 *__clib4 = (struct _clib4 *) startedTask->pr_UID; // GetEntryData();

    // we have to set the priority here to avoid race conditions
    SetTaskPri((struct Task *) startedTask, inf->attr.param.sched_priority);

    // custom stack requires special handling
    if (inf->attr.stackaddr != NULL && inf->attr.stacksize > 0) {
        stack.stk_Lower = inf->attr.stackaddr;
        stack.stk_Upper = (ULONG)((APTR) stack.stk_Lower) + inf->attr.stacksize;
        stack.stk_Pointer = (APTR) stack.stk_Upper;

        StackSwap(&stack);
        stackSwapped = TRUE;
    }

	ReplyMsg(&newThreadMessage->message);

    /* Allocate signals AFTER ReplyMsg */
    if (!inf->detached) {
	    inf->cancel_signal = AllocSignal(-1);
    	if (inf->cancel_signal == -1) {
    		inf->cancel_signal_mask = SIGBREAKF_CTRL_C;
    		D(("StarterFunc: %s AllocSignal cancel failed, fallback SIGBREAKF_CTRL_C\n", inf->name));
    	} else {
    		inf->cancel_signal_mask = 1L << inf->cancel_signal;
    		D(("StarterFunc: %s allocated cancel signal %d mask 0x%lx\n", inf->name, inf->cancel_signal, (unsigned long)inf->cancel_signal_mask));
    	}

        /* Allocate join signal */
        inf->join_signal = AllocSignal(-1);
        if (inf->join_signal == -1) {
            inf->join_signal_mask = SIGF_PARENT;
            D(("StarterFunc: %s AllocSignal join failed, fallback SIGF_PARENT\n", inf->name));
        } else {
            inf->join_signal_mask = 1L << inf->join_signal;
            D(("StarterFunc: %s allocated join signal %d mask 0x%lx\n", inf->name, inf->join_signal, (unsigned long)inf->join_signal_mask));
        }
    }

    D(("StarterFunc: thread %s about to call start function\n", inf->name));

    // set a jump point for pthread_exit
    if (!setjmp(inf->jmp)) {
        inf->status = THREAD_STATE_RUNNING;
        D(("StarterFunc: thread %s calling start function NOW\n", inf->name));
        inf->ret = inf->start(inf->arg);
        D(("StarterFunc: thread %s start function RETURNED\n", inf->name));
    }

    /* Don't acquire thread_sem here - let pthread_join register first if it's waiting */

    D(("StarterFunc: thread %s returned from start function\n", inf->name));

    // destroy all non-NULL TLS key values
    // since the destructors can set the keys themselves, we have to do multiple iterations
    for (int j = 0; keyFound && j < PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
        keyFound = FALSE;
        for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
            void *oldvalue = NULL;
            void (*destructor_func)(void *) = NULL;

            MutexObtain(tls_sem);
            if (inf->tlsvalues[i] && tlskeys[i].used && tlskeys[i].destructor) {
                D(("StarterFunc: thread %s calling destructor for key %d\n", inf->name, i));
                oldvalue = inf->tlsvalues[i];
                destructor_func = tlskeys[i].destructor;
                inf->tlsvalues[i] = NULL;
            }
            MutexRelease(tls_sem);

            /* Call destructor outside of tls_sem to avoid blocking other
             * threads that are exiting and need to run their own destructors */
            if (destructor_func) {
                destructor_func(oldvalue);
                D(("StarterFunc: thread %s destructor for key %d returned\n", inf->name, i));
                keyFound = TRUE;
            }
        }
    }

    /*  If we have timer running tasks for this thread, stop them before exit  */
    if (!IsMinListEmpty(&__clib4->tmr_real_list)) {
        uint32 currentThreadID = (uint32)FindTask(NULL);
        D(("StarterFunc: thread %s has timers, killing them\n", inf->name));
        /* Block SIGALRM signal from raise */
        sigblock(SIGALRM);
        /* Kill itimer for current thread */
        killitimer_by_thread(currentThreadID);
        D(("StarterFunc: thread %s timers killed\n", inf->name));
    }

    /* If we had swapped the stack, restore it */
    if (stackSwapped)
        StackSwap(&stack);

    /* NOW acquire thread_sem to atomically set DESTRUCT and search for joiner */
    MutexObtain(thread_sem);

    D(("StarterFunc[%s]: Acquired thread_sem for exit\n", inf->name));

    if (!inf->detached) {
        // tell the parent thread that we are done
        D(("StarterFunc: thread %s not detached, looking for joiner\n", inf->name));
        inf->status = THREAD_STATE_DESTRUCT;

        /* Find who is waiting to join with us */
        ThreadInfo *joiner = NULL;
        struct Node *node;

        D(("StarterFunc: thread %s scanning joiners list for join_id=%ld\n", inf->name, inf->thread_id));

        /* Scan the joiners list */
        for (node = (struct Node *)join_list.mlh_Head;
             node->ln_Succ != NULL;
             node = (struct Node *)node->ln_Succ) {
            joiner = (ThreadInfo *)((char *)node - offsetof(ThreadInfo, join_node));
            D(("StarterFunc: visiting joiner %s (id=%d) waiting for %d\n", joiner->name, joiner->thread_id, joiner->join_thread_id));

            /* Note: Check if thread_id matches what joiner is waiting for */
            if (joiner->join_thread_id == inf->thread_id) {
                D(("StarterFunc: thread %s found joiner (id=%d) waiting for me\n", inf->name, joiner->thread_id));

                /* Set return value in joiner's structure */
                if (inf->status == THREAD_STATE_CANCELED)
                    joiner->join_result = PTHREAD_CANCELED;
                else
                    joiner->join_result = inf->ret;

                /* Signal the joiner */
                if (joiner->join_signal_mask != 0) {
                    D(("StarterFunc: signaling joiner (task=%p) with mask=0x%lx\n", joiner->task, (unsigned long)joiner->join_signal_mask));
                    Signal((struct Task *)joiner->task, joiner->join_signal_mask);
                } else {
                    D(("StarterFunc: WARNING - joiner has no valid join signal!\n"));
                }

                /* POSIX says multiple threads joining same thread is undefined. */
                /* If we continue, we signal all waiters. */
            }
        }

        /* Note: Joinable thread exits here - pthread_join will clean up the ThreadInfo */
        /* We DO NOT remove ourselves from threads array or free resources yet */
    }

    /* Detached threads need self cleanup */
    if (inf->detached) {
        _pthread_clear_threadinfo(inf);
    }
    /* NOTE: Joinable threads do NOT cleanup themselves!
     * pthread_join will do the cleanup after getting the return value.
     * This prevents race where thread clears status before join checks it.
     * Signals are already freed above, before acquiring the lock.
     */

    /* Release lock and exit - process terminates normally */
    D(("StarterFunc[%s]: Releasing thread_sem, exiting\n", inf->name));
    MutexRelease(thread_sem);

    D(("StarterFunc: thread %s exiting\n", inf->name));

	set_tls_register(NULL);

    return RETURN_OK;
}

int
pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg) {
    ThreadInfo *inf;
    char name[NAMELEN] = {0};
    size_t oldlen;
    pthread_t threadnew;
    struct Task *thisTask = FindTask(NULL);
    struct DOSIFace *IDOS = _IDOS;
	BPTR fileIn  = BZERO;
	BPTR fileOut = BZERO;
	BPTR fileErr = BZERO;
	struct newThreadMessage *newThreadMessage = NULL;
	struct MsgPort *msgPort = NULL;

    if (thread == NULL || start == NULL)
        return EINVAL;

    // grab an empty thread slot and reserve it atomically
    MutexObtain(thread_sem);
    threadnew = GetThreadId(NULL);
    if (threadnew == PTHREAD_THREADS_MAX) {
        MutexRelease(thread_sem);
        return EAGAIN;
    }

    // Reserve the slot before releasing the lock to prevent race conditions
    inf = GetThreadInfo(threadnew);
    _pthread_clear_threadinfo(inf);
    inf->status = THREAD_STATE_RUNNING; // Prevents another GetThreadId(NULL) from returning this slot
    inf->thread_id = threadnew;  /* Save our pthread_t ID */
    MutexRelease(thread_sem);

    D(("pthread_create: slot %d reserved (task %p)\n", threadnew, inf->task));
    inf->start = start;
    inf->arg = arg;
    inf->parent = (struct Process *) thisTask;
    if (attr)
        inf->attr = *attr;
    else
        pthread_attr_init(&inf->attr);
    NewMinList(&inf->cleanup);
    inf->cancelstate = PTHREAD_CANCEL_ENABLE;
    inf->canceltype = PTHREAD_CANCEL_DEFERRED;
    inf->detached = inf->attr.detachstate == PTHREAD_CREATE_DETACHED;

    /* Signals allocated lazily in StarterFunc (thread context) */
    inf->cancel_signal = -1;
    inf->cancel_signal_mask = 0;

    msgPort = AllocSysObject(ASOT_PORT, NULL);
    if (msgPort == 0) {
        SHOWMSG("Cannot allocate message port\n");
        goto out;
    }

	newThreadMessage = AllocSysObjectTags(ASOT_MESSAGE,
		ASOMSG_Size, sizeof(struct newThreadMessage),
		ASOMSG_ReplyPort, msgPort,
		TAG_DONE);
	if (newThreadMessage == NULL) {
		SHOWMSG("Cannot allocate message\n");
		goto out;
	}


    /* Check minimum stack size */
    if (inf->attr.stacksize != 0 && inf->attr.stacksize < PTHREAD_STACK_MIN)
        inf->attr.stacksize = PTHREAD_STACK_MIN;

    // Check if we have a guardsize
    if (inf->attr.guardsize > 0)
        inf->attr.stacksize += inf->attr.guardsize;

    // let's trick CreateNewProc into allocating a larger buffer for the name
    snprintf(name, sizeof(name), "pthread id #%d", threadnew);
    oldlen = strlen(name);
    memset(name + oldlen, ' ', sizeof(name) - oldlen - 1);
    name[sizeof(name) - 1] = '\0';
    strncpy(inf->name, name, NAMELEN);

    fileIn  = DupFileHandle(Input());
    fileOut = DupFileHandle(Output());
    fileErr = DupFileHandle(ErrorOutput());
    if (!fileIn || !fileOut || !fileErr)
        goto out;

    // start the child thread
    inf->task = CreateNewProcTags(
            NP_Start,                StarterFunc,
            NP_UserData,             inf,
            inf->attr.stacksize == 0 ? TAG_IGNORE : NP_StackSize, inf->attr.stacksize,
            NP_Name,                 name,
            NP_Child,                TRUE,
            NP_Input,			     fileIn,
            NP_CloseInput,		     TRUE,
            NP_Output,			     fileOut,
            NP_CloseOutput,		     TRUE,
            NP_Error,			     fileErr,
            NP_CloseError,		     TRUE,
            NP_EntryData,			 newThreadMessage,
            TAG_DONE);

out:
    if (0 == inf->task) {
        if (fileIn)
            Close(fileIn);
        if (fileOut)
            Close(fileOut);
        if (fileErr)
            Close(fileErr);

    	if (inf->cancel_signal != -1) {
			FreeSignal(inf->cancel_signal);
			inf->cancel_signal = -1;
		}
    	if (newThreadMessage != NULL) {
    		FreeSysObject(ASOT_MESSAGE, newThreadMessage);
    		newThreadMessage = NULL;
    	}
    	if (msgPort != NULL) {
    		FreeSysObject(ASOT_PORT, msgPort);
    		msgPort = NULL;
    	}
        _pthread_clear_threadinfo(inf); // Release the reserved slot back to IDLE
        return EAGAIN;
    }

	WaitPort(msgPort);
	GetMsg(msgPort);

    *thread = threadnew;

	FreeSysObject(ASOT_MESSAGE, newThreadMessage);
	FreeSysObject(ASOT_PORT, msgPort);
	newThreadMessage = NULL;
	msgPort = NULL;

    return OK;
}
