/*
 * $Id: stdlib_main.c,v 2.00 2023-05-17 14:09:00 clib4devs Exp $
*/

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif /* EXEC_EXECBASE_H */

#include <setjmp.h>
#include <string.h>
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

#include "../shared_library/clib4.h"
#include "uuid.h"

extern int main(int arg_c, char **arg_v);
static void shared_obj_init(struct _clib4 *__clib4, BOOL init);

static void
shared_obj_init(struct _clib4 *__clib4, BOOL init) {
    ENTER();

    struct ElfIFace *IElf = __clib4->IElf;

    BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
    Elf32_Handle hSelf = (Elf32_Handle) NULL;
    if (segment_list != BZERO) {
        int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &hSelf, TAG_DONE);
        if (ret == 1) {
            if (hSelf != NULL) {
                /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                InitSHLibs(hSelf, init);
            } else {
                SHOWMSG("hSelf == NULL!");
            }
        } else {
            SHOWMSG("GetSegListInfoTags fail!");
        }
    } else {
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
        D(("Calling ctor %ld", i));
        __CTOR_LIST__[i--]();
    }
}

static void
_end_ctors(void (*__DTOR_LIST__[])(void)) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        D(("Calling dtor %ld", i));
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
        struct _clib4 *__clib4) {
    volatile LONG saved_io_err;

    ENTER();
    /* This plants the return buffer for _exit(). */
    if (setjmp(__clib4->__exit_jmp_buf) != 0) {
        D(("Back from longjmp"));
        goto out;
    }

    SHOWMSG("Initialize shared objects");
    shared_obj_init(__clib4, TRUE);

    SHOWMSG("Now invoking constructors");
    /* Go through the constructor list */
    _start_ctors(__EXT_CTOR_LIST__);
    SHOWMSG("Constructors executed correctly. Calling start_main()");

    D(("Call start_main with %ld parameters", __clib4->__argc));
    /* After all these preparations, get this show on the road... */
    exit(start_main(__clib4->__argc, __clib4->__argv));
    SHOWMSG("Done. Exit from start_main()");

out:
    /* Save the current IoErr() value in case it is needed later. */
    saved_io_err = IoErr();

    /* From this point on, don't worry about ^C checking any more. */
    __clib4->__check_abort_enabled = FALSE;

    SHOWMSG("Flush all files");
    /* Dump all currently unwritten data, especially to the console. */
    __flush_all_files(__clib4, -1);

    /* If one of the destructors drops into exit(), either directly
       or through a failed assert() call, processing will resume with
       the next following destructor. */
    (void) setjmp(__clib4->__exit_jmp_buf);
    SHOWMSG("Called setjmp(__clib4->__exit_jmp_buf)");

    /* Go through the destructor list */
    SHOWMSG("invoking external destructors in reverse order");
    _end_ctors(__EXT_DTOR_LIST__);

    SHOWMSG("Close shared objects");
    shared_obj_init(__clib4, FALSE);

    SHOWMSG("done.");

    /* Restore the IoErr() value before we return. */
    SetIoErr(saved_io_err);

    RETURN(__clib4->__exit_value);
    return __clib4->__exit_value;
}

int
_main(
        char *argstr,
        int arglen,
        int (*start_main)(int, char **),
        void (*__EXT_CTOR_LIST__[])(void),
        void (*__EXT_DTOR_LIST__[])(void)) {
    struct WBStartup *sms = NULL;
    struct Process *me;
    APTR oldClib4Data;
    int rc = RETURN_FAIL;
    struct _clib4 *__clib4 = __CLIB4;

    /* Pick up the Workbench startup message, if available. */
    me = (struct Process *) FindTask(NULL);
    if (!me->pr_CLI) {
        struct MsgPort *mp = &me->pr_MsgPort;
        WaitPort(mp);
        sms = (struct WBStartup *) GetMsg(mp);
    }

    /* Store old Clib4Data */
    oldClib4Data = (APTR) me->pr_UID;

    __clib4->__WBenchMsg = sms;

    SHOWMSG("stdlib_program_name_init");
    if (!stdlib_program_name_init()) {
        SHOWMSG("cannot initialize stdlib_program_name_init");
	    goto out;
	}

    SHOWMSG("__clib4->__WBenchMsg");
    /* If we were invoked from Workbench, set up the standard I/O streams. */
    if (__clib4->__WBenchMsg != NULL) {
        SHOWMSG("set up the standard I/O streams");
        if (wb_file_init(__clib4) < 0) {
            goto out;
        }
    }

    SHOWMSG("arg_init");
    if (!arg_init()) {
        SHOWMSG("cannot initialize arg_init");
	    goto out;
	}

    /* We can enable check abort now */
    __clib4->__check_abort_enabled = TRUE;

    /* At this point exe is fully initialized */
    __clib4->__fully_initialized = TRUE;

    SHOWMSG("Call Main");
    /* We have enough room to make the call or just don't care. */
    rc = call_main(argstr, arglen, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__, __clib4);

    /* Restore old Clib4Data */
    me->pr_UID = (uint32) oldClib4Data;

out:

    SHOWMSG("arg_exit");
    arg_exit();

    SHOWMSG("stdlib_program_name_exit");
    stdlib_program_name_exit();

    SHOWMSG("workbench_exit");
    workbench_exit();

    if (sms) {
        Forbid();
        ReplyMsg(&sms->sm_Message);
    }

    SHOWMSG("Exit from _main");

    return rc;
}
