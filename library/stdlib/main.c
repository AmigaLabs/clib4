/*
 * $Id: stdlib_main.c,v 1.36 2022-08-18 14:09:00 clib2devs Exp $
*/

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif /* EXEC_EXECBASE_H */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <proto/elf.h>

struct _clib2 *__fallback_clib2;

extern int main(int arg_c, char **arg_v);

static void shared_obj_init(struct _clib2 *__clib2);
static void shared_obj_exit(struct _clib2 *__clib2);

static void
shared_obj_exit(struct _clib2 *__clib2) {
    struct ElfIFace *IElf = __clib2->IElf;
    ENTER();

    /* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
    if (__clib2->__dl_elf_handle != NULL) {
        SHOWMSG("Invoking shared object destructors");
        InitSHLibs(__clib2->__dl_elf_handle, FALSE);
    };

    LEAVE();
}

static void
shared_obj_init(struct _clib2 *__clib2) {
    ENTER();

    struct ElfIFace *IElf = __clib2->IElf;

    BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
    if (segment_list != ZERO) {
        int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &__clib2->__dl_elf_handle, TAG_DONE);
        if (ret == 1) {
            if (__clib2->__dl_elf_handle != NULL) {
                /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                InitSHLibs(__clib2->__dl_elf_handle, TRUE);
            }
            else {
                SHOWMSG("__clib2->__dl_elf_handle == NULL!");
            }
        }
        else {
            SHOWMSG("GetSegListInfoTags fail!");
        }
    }
    else {
        SHOWMSG("GetProcSegList return ZERO!");
    }

    LEAVE();
}

static void
_start_ctors(void (*__CTOR_LIST__[])(void)) {
    int i = 0;

    while (__CTOR_LIST__[i + 1]) {
        i++;
    }
    SHOWVALUE(i);
    while (i > 0) {
        SHOWMSG("Calling ctor");
        SHOWVALUE(i);
        __CTOR_LIST__[i--]();
    }
}

static void
_end_ctors(void (*__DTOR_LIST__[])(void)) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        SHOWMSG("Calling dtor");
        SHOWVALUE(i);
        __DTOR_LIST__[i++]();
    }
}

static int
call_main(
        char *argstr,
        int arglen,
        int (*start_main)(int, char **),
        void (*__EXT_CTOR_LIST__[])(void),
        void (*__EXT_DTOR_LIST__[])(void),
        struct _clib2 *__clib2) {
    volatile LONG saved_io_err;

    ENTER();

    /* This plants the return buffer for _exit(). */
    if (setjmp(__clib2->__exit_jmp_buf) != 0) {
        D(("Back from longjmp"));
        goto out;
    }

    SHOWMSG("Initialize shared objects");
    shared_obj_init(__clib2);

    SHOWMSG("Now invoking constructors");
    /* Go through the constructor list */
    _start_ctors(__EXT_CTOR_LIST__);
    SHOWMSG("Constructors executed correctly. Calling start_main()");

    /* Set system time for rusage.
     * This can be executed only here.
     * Not in reent_init not in TIMER constructor because
     * __ITimer or __clib2 cannot be yet available
     */
    struct TimerIFace *ITimer = __clib2->__ITimer;
    if (__clib2->__ITimer != NULL) {
        SHOWMSG("Calling GetSysTime");
        GetSysTime(&__clib2->clock);
        /* Generate random seed */
        __clib2->__random_seed = time(NULL);
    }

    /* Set __current_path_name to a valid value */
    UBYTE current_dir_name[256] = {0};
    struct Process *this_process = (struct Process *) FindTask(NULL);
    if (NameFromLock(this_process->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }
    /* This can be helpful for debugging purposes: print the name of the current
       directory, followed by the name of the command and all the parameters
       passed to it. */
#ifndef NDEBUG
    {
        UBYTE value_str[10];
        LONG value;

        /* Careful: only echo this information if a global environment variable is set to enable this feature! */
        if (GetVar("_echo", (STRPTR) value_str, sizeof(value_str), GVF_GLOBAL_ONLY) > 0 &&
            StrToLong((CONST_STRPTR) value_str, &value) > 0 && value != 0) {
            STRPTR arg_str = GetArgStr();
            size_t arg_str_len = strlen((const char *) arg_str);
            UBYTE *arg_str_copy;
            arg_str_copy = AllocVecTags(arg_str_len + 1, AVT_Type, MEMF_PRIVATE, TAG_DONE);
            if (arg_str_copy != NULL) {
                strcpy((char *) arg_str_copy, (char *) arg_str);

                while (arg_str_len > 0 && arg_str_copy[arg_str_len - 1] <= ' ')
                    arg_str_copy[--arg_str_len] = '\0';

                D(("[%s] %s %s\n", current_dir_name, __clib2->__progname, arg_str_copy));
                FreeVec(arg_str_copy);
            }
        }
    }
#endif /* NDEBUG */

    D(("Call start_main with %ld parameters", __clib2->__argc));
    /* After all these preparations, get this show on the road... */
    exit(start_main(__clib2->__argc, __clib2->__argv));
    SHOWMSG("Done. Exit from start_main()");

out:
    D(("Uh.."));

    /* Save the current IoErr() value in case it is needed later. */
    saved_io_err = IoErr();

    /* From this point on, don't worry about ^C checking any more. */
    __clib2->__check_abort_enabled = FALSE;

    /* If we end up here with the __stack_overflow variable
       set then the stack overflow handler dropped into
       longjmp() and exit() did not get called. This
       means that we will have to show the error message
       and invoke exit() all on our own. */
    if (__clib2->__stack_overflow) {
        SHOWMSG("we have a stack overflow");

        /* Dump all currently unwritten data, especially to the console. */
        __flush_all_files(-1);

        __show_error("Stack overflow detected");

        if (setjmp(__clib2->__exit_jmp_buf) == 0)
            exit(RETURN_FAIL);
    }

/* If necessary, print stack size usage information. */
#ifndef NDEBUG
    {
        __stack_usage_exit();
    }
#endif /* NDEBUG */

    SHOWMSG("Flush all files");
    /* Dump all currently unwritten data, especially to the console. */
    __flush_all_files(-1);

    /* If one of the destructors drops into exit(), either directly
       or through a failed assert() call, processing will resume with
       the next following destructor. */
    (void) setjmp(__clib2->__exit_jmp_buf);
    SHOWMSG("Called setjmp(__clib2->__exit_jmp_buf)");

    /* Go through the destructor list */
    SHOWMSG("invoking external destructors in reverse order");
    _end_ctors(__EXT_DTOR_LIST__);

    SHOWMSG("Close shared objects");
    shared_obj_exit(__clib2);

    SHOWMSG("done.");

    /* Restore the IoErr() value before we return. */
    SetIoErr(saved_io_err);

    RETURN(__clib2->__exit_value);
    return __clib2->__exit_value;
}

static void
detach_cleanup(int32_t return_code, int32_t exit_data, struct ExecBase *sysBase) {
    (void) (return_code);
    (void) (exit_data);
    (void) (sysBase);

}

static ULONG get_stack_size(void) {
    struct Task *tc = FindTask(NULL);
    ULONG upper, lower;
    ULONG result;

    /* How much stack size was provided? */
    upper = (ULONG) tc->tc_SPUpper;
    lower = (ULONG) tc->tc_SPLower;
    result = upper - lower;

    return (result);
}

int
_main(
        char *argstr,
        int arglen,
        int (*start_main)(int, char **),
        void (*__CTOR_LIST__[])(void),
        void (*__DTOR_LIST__[])(void),
        void (*__EXT_CTOR_LIST__[])(void),
        void (*__EXT_DTOR_LIST__[])(void)) {
    struct Process *volatile child_process = NULL;
    struct WBStartup *volatile startup_message;
    volatile APTR old_window_pointer = NULL;
    volatile BOOL old_window_pointer_valid = FALSE;
    struct Process *this_process;
    int return_code = RETURN_FAIL;
    ULONG current_stack_size;
    struct _clib2 *__clib2;
    APTR oldClibData = NULL;

    /* Pick up the Workbench startup message, if available. */
    this_process = (struct Process *) FindTask(NULL);

    if (this_process->pr_CLI == ZERO) {
        struct MsgPort *mp = &this_process->pr_MsgPort;

        WaitPort(mp);

        startup_message = (struct WBStartup *) GetMsg(mp);

    } else {
        startup_message = NULL;
    }

    /* If all libraries are opened correctly we can initialize clib2 reent structure */
    D(("Initialize clib2 reent structure"));
    /* Initialize global structure */
    __clib2 = (struct _clib2 *) AllocVecTags(sizeof(struct _clib2),
            AVT_Type, MEMF_SHARED,
            AVT_ClearWithValue, 0,
            TAG_END);
    if (__clib2 == NULL)
        goto out;

    /* Get the current task pointer */
    __clib2->self = (struct Process *) FindTask(NULL);

    oldClibData = this_process->pr_CLibData;
    this_process->pr_CLibData = __clib2;
    SHOWPOINTER(__clib2);

    reent_init(__clib2);

    D(("Initialize __fallback_clib2 structure"));
    /* Initialize global structure */
    __fallback_clib2 = (struct _clib2 *) AllocVecTags(sizeof(struct _clib2),
            AVT_Type, MEMF_SHARED,
            AVT_ClearWithValue, 0,
            TAG_END);
    if (__fallback_clib2 == NULL)
        goto out;

    /* Get the current task pointer */
    __fallback_clib2->self = (struct Process *) FindTask(NULL);

    reent_init(__fallback_clib2);

    __clib2->__WBenchMsg = (struct WBStartup *) startup_message;

    if (__clib2->__disable_dos_requesters) {
        /* Don't display any requesters. */
        old_window_pointer = __set_process_window((APTR) - 1);
    } else {
        /* Just remember the original pointer. */
        old_window_pointer = __set_process_window(NULL);

        __set_process_window(old_window_pointer);
    }

    old_window_pointer_valid = TRUE;

    /* After reent structure we can call clib2 constructors */
    D(("Calling clib2 ctors"));
    _start_ctors(__CTOR_LIST__);
    D(("Done. All constructors called"));

    /* If a callback was provided which can fill us in on which
       minimum stack size should be used, invoke it now and
       store its result in the global __stack_size variable. */
    if (__get_default_stack_size != NULL) {
        unsigned int size = (*__get_default_stack_size)();
        if (size > 0)
            __clib2->__stack_size = size;
    }

    /* How much stack space was provided? */
    current_stack_size = get_stack_size();

    DECLARE_UTILITYBASE();
    /* Set default terminal mode to "amiga" if not set */
    char term_buffer[32] = {0};
    LONG term_len = GetVar("TERM", (STRPTR) term_buffer, 32, 0);
    if (term_len <= 0) {
        Strlcpy(term_buffer, "amiga-clib2", 5);
        SetVar("TERM", term_buffer, 5, 0);
    }

    /* The following code will be executed if the program is to keep
       running in the shell or was launched from Workbench. */
    int old_priority = this_process->pr_Task.tc_Node.ln_Pri;

    /* Change the task priority, if requested. */
    if (-128 <= __clib2->__priority && __clib2->__priority <= 127)
        SetTaskPri((struct Task *) this_process, __clib2->__priority);

    /* Was a minimum stack size requested and do we need more stack space than was provided for? */
    if (__clib2->__stack_size > 0 && current_stack_size < (ULONG) __clib2->__stack_size) {
        struct StackSwapStruct *stk;
        unsigned int stack_size;
        APTR new_stack;

        /* Make the stack size a multiple of 32 bytes. */
        stack_size = 32 + ((__clib2->__stack_size + 31UL) & ~31UL);

        /* Allocate the stack swapping data structure and the stack space separately. */
        stk = AllocVecTags(sizeof(*stk), AVT_Type, MEMF_PUBLIC | MEMF_ANY, TAG_DONE);
        if (stk == NULL)
            goto out;

        new_stack = AllocVecTags(stack_size, AVT_Type, MEMF_PUBLIC | MEMF_ANY, TAG_DONE);
        if (new_stack == NULL) {
            FreeVec(stk);
            goto out;
        }

        /* Fill in the lower and upper bounds, then take care of the stack pointer itself. */
        stk->stk_Lower = new_stack;
        stk->stk_Upper = (ULONG)(new_stack) + stack_size;
        stk->stk_Pointer = (APTR)(stk->stk_Upper - 32);

        /* If necessary, set up for stack size usage measurement. */
#ifndef NDEBUG
        {
            __stack_usage_init(stk);
        }
#endif /* NDEBUG */

        SHOWMSG("Swap Stack and Call Main");
        return_code = __swap_stack_and_call(stk, (APTR) call_main(argstr, arglen, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__, __clib2));

        FreeVec(new_stack);
        FreeVec(stk);
    } else {
        SHOWMSG("Call Main");
        /* We have enough room to make the call or just don't care. */
        return_code = call_main(argstr, arglen, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__, __clib2);
    }

    /* Restore the task priority. */
    SetTaskPri((struct Task *) this_process, old_priority);

out:

    SHOWMSG("Calling clib2 dtors");
    _end_ctors(__DTOR_LIST__);
    SHOWMSG("Done. All destructors called");

    SHOWMSG("Calling reent_exit on _clib2");
    reent_exit(__clib2);

    SHOWMSG("Calling reent_exit on __fallback_clib2");
    reent_exit(__fallback_clib2);

    this_process->pr_CLibData = oldClibData;

    if (old_window_pointer_valid)
        __set_process_window(old_window_pointer);

    if (startup_message != NULL) {
        Forbid();

        ReplyMsg((struct Message *) startup_message);

    }

    return (return_code);
}
