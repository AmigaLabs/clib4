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

    D(("%d %d\n", __clib4->processId, GetPID(NULL, GPID_PROCESS)));
    if (__clib4->processId != GetPID(NULL, GPID_PROCESS)) {
        SHOWMSG("NOT IN MAIN TASK");
        /* We are NOT in main task. Don't call exit here because
         * we'll have pending processes. Try to remove the Task
         */
        RemTask(NULL);
        LEAVE();
        __builtin_unreachable();
    }
    else {
        SHOWMSG("IN MAIN TASK");
        /*  If we have a previous timer running task stop it before raise SIGINT  */
        if (__clib4->tmr_real_task) {
            /* Block SIGALRM signal from raise */
            sigblock(SIGALRM);
            /* Kill itimer */
            killitimer();
        }

        /* Dump all currently unwritten data, especially to the console. */
        __flush_all_files(__clib4, -1);

        LEAVE();

        longjmp(__clib4->__exit_jmp_buf, 1);
        __builtin_unreachable();
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
