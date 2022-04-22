/*
 * $Id: stdlib_exit.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int NOCOMMON
__exit_value = RETURN_FAIL;
jmp_buf NOCOMMON
__exit_jmp_buf;

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

void
_exit(int return_code) {
    __exit_value = return_code;

    ENTER();
    SHOWVALUE(return_code);

    /*  If we have a previous timer running task stop it before raise SIGINT  */
    if (__global_clib2->tmr_real_task) {
        struct Hook h = {{NULL, NULL}, (HOOKFUNC) hook_function, NULL, NULL};
        int32 pid, process;

        Forbid();
        /* Block SIGALRM signal from raise */
        sigblock(SIGALRM);
        /* Get itimer process ID */
        pid = __global_clib2->tmr_real_task->pr_ProcessID;
        /* Scan for process */
        process = ProcessScan(&h, (CONST_APTR) pid, 0);
        while (process > 0) {
            /* Send a SIGBREAKF_CTRL_F signal until the timer task return in Wait and can get the signal */
            Signal((struct Task *) __global_clib2->tmr_real_task, SIGBREAKF_CTRL_F);
            process = ProcessScan(&h, (CONST_APTR) pid, 0);
            usleep(10);
        }
        WaitForChildExit(pid);
        __global_clib2->tmr_real_task = NULL;
        Permit();
    }

    /* Dump all currently unwritten data, especially to the console. */
    __flush_all_files(-1);

    LEAVE();

    longjmp(__exit_jmp_buf, 1);
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
