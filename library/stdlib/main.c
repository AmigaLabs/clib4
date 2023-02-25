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

#include <proto/elf.h>

/*
 * Dummy constructor and destructor array. The linker script will put these at the
 * very beginning of section ".ctors" and ".dtors". crtend.o contains a similar entry
 * with a NULL pointer entry and is put at the end of the sections. This way, the init
 * code can find the global constructor/destructor pointers.
 *
 * WARNING:
 * This hack does not work correctly with GCC 5 and higher. The optimizer
 * will see a one element array and act appropriately. The current workaround
 * is to use -fno-aggressive-loop-optimizations when compiling this file.
 *
 * NOTE:  In order to be able to support SVR4 shared libraries, we arrange
 * to have one set of symbols { __CTOR_LIST__, __DTOR_LIST__, __CTOR_END__,
 * __DTOR_END__ } per root executable and also one set of these symbols
 * per shared library.  So in any given whole process image, we may have
 * multiple definitions of each of these symbols.  In order to prevent
 * these definitions from conflicting with one another, and in order to
 * ensure that the proper lists are used for the initialization/finalization
 * of each individual shared library (respectively), we give these symbols
 * only internal (i.e. `static') linkage, and we also make it a point to
 * refer to only the __CTOR_END__ symbol in sh/crtend.o and the __DTOR_LIST__
 * symbol in sh/crtbegin.o, where they are defined.  */

static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));

extern int main(int arg_c, char **arg_v);

BOOL open_libraries(void);
void close_libraries(void);
void _init(void);
void _fini(void);

/* This will be set to TRUE in case a stack overflow was detected. */
BOOL NOCOMMON __stack_overflow;
extern struct _clib2 NOCOMMON *__global_clib2;

extern struct Library *__ElfBase;
extern struct ElfIFace *__IElf;

#define MIN_OS_VERSION 52

void _init(void) {
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

void _fini(void) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        __DTOR_LIST__[i++]();
    }
}

void
close_libraries(void) {
    if (__IElf != NULL) {
        DropInterface((struct Interface *) __IElf);
        __IElf = NULL;
    }

    if (__IUtility != NULL) {
        DropInterface((struct Interface *) __IUtility);
        __IUtility = NULL;
    }

    if (IDOS != NULL) {
        DropInterface((struct Interface *) IDOS);
        IDOS = NULL;
    }

    if (__ElfBase != NULL) {
        CloseLibrary(__ElfBase);
        __ElfBase = NULL;
    }

    if (__UtilityBase != NULL) {
        CloseLibrary(__UtilityBase);
        __UtilityBase = NULL;
    }

    if (DOSBase != NULL) {
        CloseLibrary(DOSBase);
        DOSBase = NULL;
    }
}

static int
call_main(void) {
    volatile LONG saved_io_err;

    ENTER();

    /* This plants the return buffer for _exit(). */
    if (setjmp(__exit_jmp_buf) != 0) {
        goto out;
    }

    reent_init();

    SHOWMSG("now invoking the constructors");
    /* Go through the constructor list */
    _init();

    SHOWMSG("Constructors executed correctly. Calling main()");

    /* This can be helpful for debugging purposes: print the name of the current
       directory, followed by the name of the command and all the parameters
       passed to it. */
#ifndef NDEBUG
    {
        UBYTE value_str[10];
        LONG value;

        /* Careful: only echo this information if a global environment
                    variable is set to enable this feature! */
        if (GetVar("_echo", (STRPTR) value_str, sizeof(value_str), GVF_GLOBAL_ONLY) > 0 &&
            StrToLong((CONST_STRPTR) value_str, &value) > 0 && value != 0) {
            struct Process *this_process = (struct Process *) FindTask(NULL);
            STRPTR arg_str = GetArgStr();
            size_t arg_str_len = strlen((const char *) arg_str);
            UBYTE *arg_str_copy;
            UBYTE current_dir_name[256] = {0};
            arg_str_copy = AllocVecTags(arg_str_len + 1, AVT_Type, MEMF_PRIVATE, TAG_DONE);
            if (arg_str_copy != NULL &&
                NameFromLock(this_process->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
                strcpy((char *) arg_str_copy, (char *) arg_str);

                while (arg_str_len > 0 && arg_str_copy[arg_str_len - 1] <= ' ')
                    arg_str_copy[--arg_str_len] = '\0';

                kprintf("[%s] %s %s\n", current_dir_name, __program_name, arg_str_copy);
            }

            FreeVec(arg_str_copy);
        }
    }
#endif /* NDEBUG */
    /* After all these preparations, get this show on the road... */
    exit(main((int) __argc, (char **) __argv));

out:

    /* Save the current IoErr() value in case it is needed later. */
    saved_io_err = IoErr();

    /* From this point on, don't worry about ^C checking any more. */
    __check_abort_enabled = FALSE;

    /* If we end up here with the __stack_overflow variable
       set then the stack overflow handler dropped into
       longjmp() and exit() did not get called. This
       means that we will have to show the error message
       and invoke exit() all on our own. */
    if (__stack_overflow) {
        SHOWMSG("we have a stack overflow");

        /* Dump all currently unwritten data, especially to the console. */
        __flush_all_files(-1);

        __show_error("Stack overflow detected");

        if (setjmp(__exit_jmp_buf) == 0)
            exit(RETURN_FAIL);
    }

/* If necessary, print stack size usage information. */
#ifndef NDEBUG
    {
        __stack_usage_exit();
    }
#endif /* NDEBUG */

    /* Dump all currently unwritten data, especially to the console. */
    __flush_all_files(-1);

    SHOWMSG("invoking the destructors");

    /* If one of the destructors drops into exit(), either directly
       or through a failed assert() call, processing will resume with
       the next following destructor. */
    (void) setjmp(__exit_jmp_buf);

    /* Go through the destructor list */
    _fini();

    disableUnixPaths();

    reent_exit();

    SHOWMSG("done.");

    /* Restore the IoErr() value before we return. */
    SetIoErr(saved_io_err);

    RETURN(__exit_value);
    return (__exit_value);
}

BOOL
open_libraries(void) {
    BOOL success = FALSE;

    /* Open the minimum required libraries. */
    DOSBase = (struct Library *) OpenLibrary("dos.library", MIN_OS_VERSION);
    if (DOSBase == NULL)
        goto out;

    __UtilityBase = OpenLibrary("utility.library", MIN_OS_VERSION);
    if (__UtilityBase == NULL)
        goto out;

    /* Obtain the interfaces for these libraries. */
    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, 0);
    if (IDOS == NULL)
        goto out;

    __IUtility = (struct UtilityIFace *) GetInterface(__UtilityBase, "main", 1, 0);
    if (__IUtility == NULL)
        goto out;

    /* We need elf.library V52.2 or higher. */
    __ElfBase = OpenLibrary("elf.library", 0);
    if (__ElfBase == NULL || (__ElfBase->lib_Version < MIN_OS_VERSION) ||
        (__ElfBase->lib_Version == MIN_OS_VERSION && __ElfBase->lib_Revision < 2))
        goto out;

    __IElf = (struct ElfIFace *) GetInterface(__ElfBase, "main", 1, NULL);
    if (__IElf == NULL)
        goto out;

    success = TRUE;

    out:

    return (success);
}

static void
detach_cleanup(int32_t return_code, int32_t exit_data, struct ExecBase *sysBase) {
    (void) (return_code);
    (void) (exit_data);
    (void) (sysBase);

    _fini();
}

static ULONG get_stack_size(void) {
    struct Task *tc = FindTask(NULL);
    ULONG upper, lower;
    ULONG result;

    /* How much stack size was provided? */
    upper = (ULONG)
    tc->tc_SPUpper;
    lower = (ULONG)
    tc->tc_SPLower;

    result = upper - lower;

    return (result);
}

int
_main(char *args, int arglen, struct ExecBase *sysBase) {
    struct Process *volatile child_process = NULL;
    struct WBStartup *volatile startup_message;
    volatile APTR old_window_pointer = NULL;
    volatile BOOL old_window_pointer_valid = FALSE;
    struct Process *this_process;
    int return_code = RETURN_FAIL;
    ULONG current_stack_size;
    (void) (args);
    (void) (arglen);

    SysBase = (struct Library *) sysBase;
    IExec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;

    /* Pick up the Workbench startup message, if available. */
    this_process = (struct Process *) FindTask(NULL);

    if (this_process->pr_CLI == ZERO) {
        struct MsgPort *mp = &this_process->pr_MsgPort;

        WaitPort(mp);

        startup_message = (struct WBStartup *) GetMsg(mp);
    } else {
        startup_message = NULL;
    }

    __WBenchMsg = (struct WBStartup *) startup_message;

    /* Try to open the libraries we need to proceed. */
    if (CANNOT open_libraries()) {
        const char *error_message;

        /* If available, use the error message provided by the client. */
        error_message = __minimum_os_lib_error;

        if (error_message == NULL)
            error_message = "This program requires AmigaOS 4.0 (52.2) or higher.";

        __show_error(error_message);
        goto out;
    }

    if (__disable_dos_requesters) {
        /* Don't display any requesters. */
        old_window_pointer = __set_process_window((APTR) - 1);
    } else {
        /* Just remember the original pointer. */
        old_window_pointer = __set_process_window(NULL);

        __set_process_window(old_window_pointer);
    }

    old_window_pointer_valid = TRUE;

    /* If a callback was provided which can fill us in on which
       minimum stack size should be used, invoke it now and
       store its result in the global __stack_size variable. */
    if (__get_default_stack_size != NULL) {
        unsigned int size = (*__get_default_stack_size)();
        if (size > 0)
            __stack_size = size;
    }

    /* How much stack space was provided? */
    current_stack_size = get_stack_size();

    /* If this is a resident program, don't allow for the detach
       code to run. Same goes for launching the program from
       Workbench. */
    if (__is_resident || startup_message != NULL) {
        __detach = FALSE;
    } else if (__check_detach != NULL) {
        /* Check if we may need to detach from the shell. */
        __detach = (*__check_detach)();
    }

    /* Set default terminal mode to "amiga" if not set */
    char term_buffer[32] = {0};
    LONG term_len = GetVar("TERM", (STRPTR) term_buffer, 32, 0);
    if (term_len <= 0) {
        strncpy(term_buffer, "amiga-clib2", 5);
        SetVar("TERM", term_buffer, 5, 0);
    }

    /* The following code will be executed if the program is to keep
       running in the shell or was launched from Workbench. */
    if (DO_NOT __detach) {
        int old_priority = this_process->pr_Task.tc_Node.ln_Pri;

        /* Change the task priority, if requested. */
        if (-128 <= __priority && __priority <= 127)
            SetTaskPri((struct Task *) this_process, __priority);

        /* Was a minimum stack size requested and do we need more stack space than was provided for? */
        if (__stack_size > 0 && current_stack_size < (ULONG)__stack_size)
        {
            struct StackSwapStruct *stk;
            unsigned int stack_size;
            APTR new_stack;

            /* Make the stack size a multiple of 32 bytes. */
            stack_size = 32 + ((__stack_size + 31UL) & ~31UL);

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

            return_code = __swap_stack_and_call(stk, (APTR) call_main);

            FreeVec(new_stack);
            FreeVec(stk);
        }
        else
        {
            /* We have enough room to make the call or just don't care. */
            return_code = call_main();
        }

        /* Restore the task priority. */
        SetTaskPri((struct Task *) this_process, old_priority);
    } else {
        struct CommandLineInterface *cli = Cli();
        struct TagItem tags[12];
        TEXT program_name[256] = {0};
        unsigned int stack_size;
        int i;

        /* Now for the interesting part: detach from the shell we're
           currently executing in. This works only if the program is
           not reentrant and has not been launched from Workbench. */

        stack_size = __stack_size;

        if (stack_size < current_stack_size)
            stack_size = current_stack_size;

        if (stack_size < cli->cli_DefaultStack * sizeof(LONG))
            stack_size = cli->cli_DefaultStack * sizeof(LONG);

        GetCliProgramName(program_name, (LONG)
        sizeof(program_name));

        i = 0;

        tags[i].ti_Tag = NP_Entry;
        tags[i++].ti_Data = (ULONG)
        call_main;
        tags[i].ti_Tag = NP_StackSize;
        tags[i++].ti_Data = stack_size;
        tags[i].ti_Tag = NP_Name;
        tags[i++].ti_Data = (ULONG)(__process_name != NULL ? __process_name : (char *) FilePart(program_name));
        tags[i].ti_Tag = NP_CommandName;
        tags[i++].ti_Data = (ULONG)
        FilePart(program_name);
        tags[i].ti_Tag = NP_Cli;
        tags[i++].ti_Data = TRUE;
        tags[i].ti_Tag = NP_Child;
        tags[i++].ti_Data = TRUE;
        tags[i].ti_Tag = NP_Arguments;
        tags[i++].ti_Data = (ULONG)
        GetArgStr();
        tags[i].ti_Tag = NP_FinalCode;
        tags[i++].ti_Data = (ULONG)
        detach_cleanup;
        tags[i].ti_Tag = NP_FinalData;
        tags[i++].ti_Data = (ULONG)
        cli->cli_Module;

        /* Use a predefined task priority, if requested. */
        if (-128 <= __priority && __priority <= 127) {
            tags[i].ti_Tag = NP_Priority;
            tags[i++].ti_Data = (ULONG)
            __priority;
        }

        /* dos.library V50 will free the segment list upon exit. */
        if (((struct Library *) DOSBase)->lib_Version >= 50) {
            tags[i].ti_Tag = NP_Seglist;
            tags[i++].ti_Data = (ULONG)
            cli->cli_Module;;
            tags[i].ti_Tag = NP_FreeSeglist;
            tags[i++].ti_Data = TRUE;
        }

        tags[i].ti_Tag = TAG_END;

        Forbid();

        child_process = CreateNewProc(tags);
        if (child_process == NULL) {
            Permit();

            PrintFault(IoErr(), program_name);

            return_code = RETURN_FAIL;
            goto out;
        }

        /* The standard I/O streams are no longer attached to a console. */
        __no_standard_io = TRUE;

        cli->cli_Module = ZERO;

        return_code = RETURN_OK;

        Permit();
    }

out:

    if (old_window_pointer_valid)
        __set_process_window(old_window_pointer);

    if (child_process == NULL)
        close_libraries();

    if (startup_message != NULL) {
        Forbid();

        ReplyMsg((struct Message *) startup_message);

    }

    return (return_code);
}
