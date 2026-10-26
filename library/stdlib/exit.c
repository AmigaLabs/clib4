/*
 * $Id: stdlib_exit.c,v 1.6 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "pthread/common.h"

void
_exit(int return_code) {
    struct _clib4 *__clib4 = __CLIB4;
    __clib4->__exit_value = return_code;

    ENTER();
    SHOWVALUE(__clib4->__exit_value);

    /*  If we have timer running tasks for this thread, stop them before raise SIGINT  */
    if (!IsMinListEmpty(&__clib4->tmr_real_list)) {
        /* Block SIGALRM signal from raise */
        sigblock(SIGALRM);
        /* Kill itimer for current thread */
        killitimer();
    }

    D(("%d %d\n", __clib4->processId, GetPID(NULL, GPID_PROCESS)));
    if (__clib4->processId != (pid_t) GetPID(NULL, GPID_PROCESS)) {
        SHOWMSG("NOT IN MAIN TASK");
        /* We are NOT in the main task, and the process ends as a whole
         * (POSIX: _exit() terminates the process, whichever thread calls
         * it). Its stack and exit point belong to the main process, so the
         * main process is asked to leave through its own exit path at its
         * next __check_abort_f() or pthread_join(), and woken. */
        __clib4->__exit_requested = TRUE;

        /* With libpthread, the hook wakes the main thread it recorded and
         * ends this thread as a pthread, so a pthread_join() waiting for it
         * returns and the exit-time join of every thread does not wait for
         * it forever. The main process may already be in that join: the
         * hook stays set until every thread has ended, since the join
         * cannot finish before this one does. The hook returns for a task
         * that is not a pthread. Without libpthread the main process is
         * woken here. */
        if (__clib4->__thread_exit_hook != NULL)
            __clib4->__thread_exit_hook();
        else
            Signal((struct Task *) __clib4->self, SIGBREAKF_CTRL_C);

        RemTask(NULL);
        LEAVE();
        __builtin_unreachable();
    }
    else {
        SHOWMSG("IN MAIN TASK");

        /* Dump all currently unwritten data, especially to the console. */
        __flush_all_files(__clib4, -1);

        LEAVE();

        if (__clib4->__exit_jmp_buf_valid) {
            longjmp(__clib4->__exit_jmp_buf, 1);
            __builtin_unreachable();
        }
        else {
            /* jmp_buf was never initialized; safely terminate the task */
        }
    }
}

/* The C99 version of _exit(). */
void
_Exit(int return_code) {
    _exit(return_code);
}

void
exit(int return_code) {
    __exit_trap_trigger();
    _exit(return_code);
}
