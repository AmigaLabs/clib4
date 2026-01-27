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

static ThreadInfo *old_tls = NULL;

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

    old_tls = get_tls_register();

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
        // Check if we have a guardsize
        if (inf->attr.guardsize > 0)
            inf->attr.stacksize += inf->attr.guardsize;

        stack.stk_Lower = inf->attr.stackaddr;
        stack.stk_Upper = (ULONG)((APTR) stack.stk_Lower) + inf->attr.stacksize;
        stack.stk_Pointer = (APTR) stack.stk_Upper;

        StackSwap(&stack);
        stackSwapped = TRUE;
    }

	ReplyMsg(&newThreadMessage->message);

    /* Allocate signals AFTER ReplyMsg (like pthreads.library ThreadCode/init does after replying to BirthMessage) */
    if (!inf->detached) {
	    inf->cancel_signal = AllocSignal(-1);
    	if (inf->cancel_signal == -1) {
    		inf->cancel_signal_mask = SIGBREAKF_CTRL_C;
    		D(("StarterFunc: %s AllocSignal cancel failed, fallback SIGBREAKF_CTRL_C\n", inf->name));
    	} else {
    		inf->cancel_signal_mask = 1L << inf->cancel_signal;
    		D(("StarterFunc: %s allocated cancel signal %d mask 0x%lx\n", inf->name, inf->cancel_signal, (unsigned long)inf->cancel_signal_mask));
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
    MutexObtain(tls_sem);
    for (int j = 0; keyFound && j < PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
        keyFound = FALSE;
        for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
            if (inf->tlsvalues[i] && tlskeys[i].used && tlskeys[i].destructor) {
                D(("StarterFunc: thread %s calling destructor for key %d\n", inf->name, i));
                void *oldvalue = inf->tlsvalues[i];
                void (*destructor_func)(void *) = tlskeys[i].destructor;
                inf->tlsvalues[i] = NULL;
                /* Save destructor to local variable to prevent race with pthread_key_delete
                 * which could set it to NULL between the check and the call */
                if (destructor_func) {
                    destructor_func(oldvalue);
                }
                D(("StarterFunc: thread %s destructor for key %d returned\n", inf->name, i));
                keyFound = TRUE;
            }
        }
    }
    MutexRelease(tls_sem);

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

    if (stackSwapped)
        StackSwap(&stack);

    /* NOW acquire thread_sem to atomically set DESTRUCT and search for joiner */
    MutexObtain(thread_sem);

    if (!inf->detached) {
        // tell the parent thread that we are done (like pthreads.library: find joiner and signal)
        D(("StarterFunc: thread %s not detached, looking for joiner\n", inf->name));
        inf->status = THREAD_STATE_DESTRUCT;

        /* Find who is waiting to join with us */
        pthread_t my_id = inf->thread_id;  /* Use saved thread_id, not pointer arithmetic */
        ThreadInfo *joiner = NULL;

        D(("StarterFunc: thread %s searching for joiner, my_id=%ld\n", inf->name, my_id));

        for (int i = 0; i < PTHREAD_THREADS_MAX; i++) {
            if (threads[i].join_thread_id == my_id && threads[i].join_signal_mask != 0) {
                joiner = &threads[i];
                D(("StarterFunc: thread %s found joiner thread %d, signaling mask 0x%lx\n", inf->name, i, (unsigned long)joiner->join_signal_mask));
                Signal((struct Task *)joiner->task, joiner->join_signal_mask);
                break;
            }
        }

        if (!joiner) {
            /* No joiner yet - just mark as DESTRUCT and exit */
            /* pthread_join will find us later in this state and clean up */
            D(("StarterFunc: thread %s (id=%ld) no joiner found, exiting anyway\n", inf->name, my_id));
        } else {
            /* Joiner found and signaled */
            D(("StarterFunc: thread %s joiner signaled\n", inf->name));
        }

        /* Joinable thread exits here - pthread_join will clean up the ThreadInfo */
    }

    /* Detached threads need self cleanup */
    if (inf->detached) {
        _pthread_clear_threadinfo(inf);
    }
    /* NOTE: Joinable threads do NOT cleanup themselves!
     * pthread_join will do the cleanup after getting the return value.
     * This prevents race where thread clears status before join checks it.
     */

    /* Release lock and exit - process terminates normally */
    MutexRelease(thread_sem);

    D(("StarterFunc: thread %s exiting\n", inf->name));
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

    // grab an empty thread slot
    MutexObtain(thread_sem);
    threadnew = GetThreadId(NULL);
    MutexRelease(thread_sem);

    if (threadnew == PTHREAD_THREADS_MAX) {
        return EAGAIN;
    }

    // prepare the ThreadInfo structure
    inf = GetThreadInfo(threadnew);
    _pthread_clear_threadinfo(inf);
    D(("pthread_create: slot %d cleared (task %p)\n", threadnew, inf->task));

    inf->thread_id = threadnew;  /* Save our pthread_t ID */
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

    /* Signals allocated lazily in StarterFunc (thread context) like pthreads.library */
    inf->parent_signal = -1;
    inf->parent_signal_mask = 0;
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

    	if (inf->parent_signal != -1) {
    		FreeSignal(inf->parent_signal);
    		inf->parent_signal = -1;
    	}
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
        inf->parent = NULL;
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
