/*
 * $Id: libc_init_global.c,v 2.0 2023-05-17 17:01:06 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include "locale/dcngettext.h"
#include <syslog.h>

#include "debug.h"

#include <proto/elf.h>
#include <fenv.h>

extern struct ElfIFace *__IElf;
extern struct Library *__ElfBase;

/* random table */
static uint32_t _random_init[] = {
        0x00000000, 0x5851f42d, 0xc0b18ccf, 0xcbb5f646,
        0xc7033129, 0x30705b04, 0x20fd5db4, 0x9a8b7f78,
        0x502959d8, 0xab894868, 0x6c0356a7, 0x88cdb7ff,
        0xb477d43f, 0x70a3a52b, 0xa8e4baf1, 0xfd8341fc,
        0x8ae16fd9, 0x742d2f7a, 0x0d1f0796, 0x76035e09,
        0x40f7702c, 0x6fa72ca5, 0xaaa84157, 0x58a0df74,
        0xc74a0364, 0xae533cc4, 0x04185faf, 0x6de3b115,
        0x0cab8628, 0xf043bfa4, 0x398150e9, 0x37521657
};

void
reent_init(struct _clib4 *__clib4, const BOOL fallback) {
    BOOL success = FALSE;

    ENTER();
    DECLARE_UTILITYBASE();

    struct ElfIFace *IElf = __IElf;

    *__clib4 = (struct _clib4) {
        /* Set main Exec and IElf interface pointers */
        .IExec = IExec,
        .IElf = __IElf,
        .__WBenchMsg = NULL,
        .input = BZERO,
        .output = BZERO,
        .error = BZERO,
        /* Disable check abort at start */
        .__check_abort_enabled = FALSE,
        .__break_signal_mask = SIGBREAKF_CTRL_C,
        ._errno = 0,
        .__shell_escape_character = '*',
        .__cache_line_size = 32,
        /* We use an invalid priority value to indicate "do not change the program's task priority". */
        .__priority = 256,
        .pipenum = 0,
        .tgoto_buf = {0},
        /* Initialize pipe semaphore */
        .__pipe_semaphore = __create_semaphore(),
        /* Initialize random signal and state */
        .__random_lock = __create_semaphore(),
        .n = 31,
        .i = 3,
        .j = 0,
        .x = _random_init + 1,
        /* Initialize getrandom fd */
        .randfd[0] = -1,
        .randfd[1] = -1,
        ._gamma_signgam = 0,
        .__infinity = 0,
        .__nan = 0,
        /* Set locale stuff */
        ._current_category = LC_ALL,
        ._current_locale = "C-UTF-8",
        .__mb_cur_max = 1,
        .__disable_dos_requesters = FALSE,
        .__expand_wildcard_args = TRUE,
        .__unlink_retries = TRUE,
        /* Clear itimer start time */
        .tmr_start_time.tv_sec = 0,
        .tmr_start_time.tv_usec = 0,
        .tmr_real_task = NULL,
        /* Set ar4random stuff */
        .rs.i = 0,
        .rs.j = 0,
        .rs_initialized = 0,
        .rs_stired = 0,
        .arc4_count = 0,
        .rs_data_available = 0,
        .__default_path_delimiter = ":",
        .__default_path = "/C:.:/APPDIR:/PROGDIR:/ram:/SDK/C:/SDK/Local/C:",
        /* Default root groups and id */
        .__root_mode = FALSE,
        .__root_uid = 0,
        .__root_gid = 0,
        .__root_euid = 0,
        .__root_egid = 0,

        .__tzname[0] = NULL,
        .__tzname[1] = NULL,
        .__daylight = 0,
        .__timezone = 0,

        /* Default debug levels */
        .indent_level = 0,
        .previous_debug_level = -1,
        .__debug_level = DEBUGLEVEL_CallTracing,
        .g_mofile = NULL,
        .__ospeed = 0,
        .__tputs_baud_rate = 0,
        .__PC = '\0',
        /* Syslog stuff */
        .syslog_fd = NULL,
        .syslog_openlog_flags = 0,
        .syslog_facility = LOG_USER,
        .syslog_mask = 0xff,
        .__fully_initialized = FALSE,
        .__children = 1,
        .term_entry = NULL,
        .__was_sig = -1,
        .__wof_mem_allocator_type = WMEM_ALLOCATOR_SIMPLE,
        .allocated_memory_by_malloc = 0,
        .__environment_pool = NULL,
        .__num_iob = 0,
        ._iob_pool = NULL,
        .isTZSet = 0,
        .__IDebug = NULL,
    };

    if (!__clib4->__random_lock || !__clib4->__pipe_semaphore) {
        goto out;
    }

    SHOWMSG("Allocating file IO pool");
    __clib4->_iob_pool = AllocSysObjectTags(ASOT_MEMPOOL,
                                               ASOPOOL_Puddle,		BUFSIZ + 32,
                                               ASOPOOL_Threshold,	BUFSIZ + 32,
                                               TAG_DONE);
    if (!__clib4->_iob_pool) {
        goto out;
    }
    DebugPrintF("Allocated file IO pool. _iob_pool : 0x%lx\n", __clib4->_iob_pool);

    SHOWMSG("Allocating wide_status");
    /* Initialize wchar stuff */
    __clib4->wide_status = AllocVecTags(sizeof(struct _wchar), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (!__clib4->wide_status) {
        goto out;
    }
    __clib4->wide_status->_strtok_last = NULL;
    __clib4->wide_status->_mblen_state.__count = 0;
    __clib4->wide_status->_mblen_state.__value.__wch = 0;
    __clib4->wide_status->_wctomb_state.__count = 0;
    __clib4->wide_status->_wctomb_state.__value.__wch = 0;
    __clib4->wide_status->_mbtowc_state.__count = 0;
    __clib4->wide_status->_mbtowc_state.__value.__wch = 0;
    __clib4->wide_status->_mbrlen_state.__count = 0;
    __clib4->wide_status->_mbrlen_state.__value.__wch = 0;
    __clib4->wide_status->_mbrtowc_state.__count = 0;
    __clib4->wide_status->_mbrtowc_state.__value.__wch = 0;
    __clib4->wide_status->_mbsrtowcs_state.__count = 0;
    __clib4->wide_status->_mbsrtowcs_state.__value.__wch = 0;
    __clib4->wide_status->_wcrtomb_state.__count = 0;
    __clib4->wide_status->_wcrtomb_state.__value.__wch = 0;
    __clib4->wide_status->_wcsrtombs_state.__count = 0;
    __clib4->wide_status->_wcsrtombs_state.__value.__wch = 0;
    __clib4->wide_status->_l64a_buf[0] = '\0';
    __clib4->wide_status->_getdate_err = 0;

    ClearMem(__clib4->action_array, NSIG * sizeof(struct sigaction));

    /* Get cpu family used to choose functions at runtime */
    D(("Setting cpu family"));
    GetCPUInfoTags(GCIT_Family, &__clib4->cpufamily, TAG_DONE);

    /* Check if altivec is present */
    D(("Check if altivec is present"));
    GetCPUInfoTags(GCIT_VectorUnit, &__clib4->hasAltivec, TAG_DONE);

    __clib4->__IDebug = (struct DebugIFace *) GetInterface((struct Library *) IExec->Data.LibBase, "debug", 1, NULL);
    if (!__clib4->__IDebug) {
        D(("Cannot get IDebug interface"));
        goto out;
    }

    /*
     * Next: Get Elf handle associated with the currently running process.
     * ElfBase is opened in crtbegin.c that is called before the
     * call_main()
     */

    D(("Try to get elf handle for dl* operations"));
    if (__clib4->IElf != NULL) {
        D(("Calling GetProcSegList"));
        const BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
        if (segment_list != BZERO) {
            Elf32_Handle handle = NULL;

            D(("Calling GetSegListInfoTags"));
            if (GetSegListInfoTags(segment_list, GSLI_ElfHandle, &handle, TAG_DONE) == 1) {
                if (handle != NULL) {
                    D(("Calling OpenElfTags"));
                    __clib4->__dl_root_handle = OpenElfTags(OET_ElfHandle, handle, TAG_DONE);
                    SHOWPOINTER(__clib4->__dl_root_handle);
                }
            }
        }
    }

    ClearMem(&__clib4->tmr_time, sizeof(struct itimerval));

    /* Set ar4random stuff */
    for (int i = 0; i <= 255; i++) {
        __clib4->rs.s[i] = i;
    }

    /* Check if .unix file exists in the current dir. If the file exists enable unix path semantics */
    D(("Check for .unix file"));
    __clib4->__unix_path_semantics = FALSE;
    struct ExamineData *exd = ExamineObjectTags(EX_StringNameInput, (CONST_STRPTR) "PROGDIR:.unix", TAG_DONE);
    if (exd != NULL) {
        if (EXD_IS_FILE(exd)) {
            SHOWMSG("Enable unix paths");
            __clib4->__unix_path_semantics = TRUE;
        }
        FreeDosObject(DOS_EXAMINEDATA, exd);
    }

    /* This table holds pointers to all signal handlers configured at a time. */
    SHOWMSG("Set signal table to SIG_DFL");
    for (int i = 0; i < NSIG; i++) {
        __clib4->__signal_handler_table[i] = SIG_DFL;
    }

    success = TRUE;

out:

    if (!success) {
        reent_exit(__clib4);
    }
}

void
reent_exit(struct _clib4 *__clib4) {
    /* Free global clib structure */
    if (__clib4) {
        /* Free IO memory pool */
        if (__clib4->_iob_pool != NULL) {
            SHOWMSG("Freeing _iob_pool and all unfreed memory");
            DebugPrintF("Freeing _iob_pool and all unfreed memory. _iob_pool : 0x%lx\n", __clib4->_iob_pool);
            FreeSysObject(ASOT_MEMPOOL, __clib4->_iob_pool);
        }

        /* Free wchar stuff */
        if (__clib4->wide_status != NULL) {
            SHOWMSG("Freeing wide_status");
            FreeVec(__clib4->wide_status);
            __clib4->wide_status = NULL;
        }

        /* Drop IDebug interface */
        if (__clib4->__IDebug)
            DropInterface((struct Interface *) __clib4->__IDebug);

        /* Remove random semaphore */
        SHOWMSG("Delete __random_lock semaphore");
        __delete_semaphore(__clib4->__random_lock);
        /* Remove pipe semaphore */
        SHOWMSG("Delete __pipe_semaphore semaphore");
        __delete_semaphore(__clib4->__pipe_semaphore);
        /* Free dl stuff */
        const struct ElfIFace *IElf = __IElf;

        if (IElf && __clib4->__dl_root_handle != NULL) {
            SHOWMSG("Closing __dl_root_handle");
            CloseElfTags(__clib4->__dl_root_handle, CET_ReClose, TRUE, TAG_DONE);
            __clib4->__dl_root_handle = NULL;
            SHOWMSG("Done");
        }

        FreeVec(__clib4);
        __clib4 = NULL;
        SHOWMSG("__clib4 destroyed correctly");
    }
}

void enableUnixPaths(void) {
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->__unix_path_semantics = TRUE;

    /* Set __current_path_name to a valid value */
    const UBYTE current_dir_name[256] = {0};
    if (NameFromLock(__clib4->self->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }
}

void disableUnixPaths(void) {
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->__unix_path_semantics = FALSE;

    /* Set __current_path_name to a valid value */
    const UBYTE current_dir_name[256] = {0};
    if (NameFromLock(__clib4->self->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }
}

int *__mb_cur_max(void) {
    return &__CLIB4->__mb_cur_max;
}

