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

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process) {
    uint32 pid = (uint32) userdata;
    (void) (hook);

    if (process->pr_ProcessID == pid) {
        return process;
    }

    return 0;
}

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
reent_init(struct _clib4 *__clib4) {
    BOOL success = FALSE;

    ENTER();
    DECLARE_UTILITYBASE();

    struct ElfIFace *IElf = __IElf;

    *__clib4 = (struct _clib4) {
        .__ctype_table = {
                /*   0      */ __CTYPE_CONTROL,
                /*   1      */ __CTYPE_CONTROL,
                /*   2      */ __CTYPE_CONTROL,
                /*   3      */ __CTYPE_CONTROL,
                /*   4      */ __CTYPE_CONTROL,
                /*   5      */ __CTYPE_CONTROL,
                /*   6      */ __CTYPE_CONTROL,
                /*   7      */ __CTYPE_CONTROL,
                /*   8      */ __CTYPE_CONTROL,
                /*   9      */ __CTYPE_CONTROL|__CTYPE_WHITE_SPACE,
                /*  10      */ __CTYPE_CONTROL|__CTYPE_WHITE_SPACE,
                /*  11      */ __CTYPE_CONTROL|__CTYPE_WHITE_SPACE,
                /*  12      */ __CTYPE_CONTROL|__CTYPE_WHITE_SPACE,
                /*  13      */ __CTYPE_CONTROL|__CTYPE_WHITE_SPACE,
                /*  14      */ __CTYPE_CONTROL,
                /*  15      */ __CTYPE_CONTROL,
                /*  16      */ __CTYPE_CONTROL,
                /*  17      */ __CTYPE_CONTROL,
                /*  18      */ __CTYPE_CONTROL,
                /*  19      */ __CTYPE_CONTROL,
                /*  20      */ __CTYPE_CONTROL,
                /*  21      */ __CTYPE_CONTROL,
                /*  22      */ __CTYPE_CONTROL,
                /*  23      */ __CTYPE_CONTROL,
                /*  24      */ __CTYPE_CONTROL,
                /*  25      */ __CTYPE_CONTROL,
                /*  26      */ __CTYPE_CONTROL,
                /*  27      */ __CTYPE_CONTROL,
                /*  28      */ __CTYPE_CONTROL,
                /*  29      */ __CTYPE_CONTROL,
                /*  30      */ __CTYPE_CONTROL,
                /*  31      */ __CTYPE_CONTROL,
                /*  32, ' ' */ __CTYPE_PRINTABLE|__CTYPE_WHITE_SPACE,
                /*  33, '!' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  34, '"' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  35, '#' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  36, '$' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  37, '%' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  38, '&' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  39, ''' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  40, '(' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  41, ')' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  42, '*' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  43, '+' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  44, ',' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  45, '-' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  46, '.' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  47, '/' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  48, '0' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  49, '1' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  50, '2' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  51, '3' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  52, '4' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  53, '5' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  54, '6' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  55, '7' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  56, '8' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  57, '9' */ __CTYPE_DIGIT|__CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE,
                /*  58, ':' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  59, ';' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  60, '<' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  61, '=' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  62, '>' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  63, '?' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  64, '@' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  65, 'A' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  66, 'B' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  67, 'C' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  68, 'D' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  69, 'E' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  70, 'F' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  71, 'G' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  72, 'H' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  73, 'I' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  74, 'J' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  75, 'K' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  76, 'L' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  77, 'M' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  78, 'N' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  79, 'O' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  80, 'P' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  81, 'Q' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  82, 'R' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  83, 'S' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  84, 'T' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  85, 'U' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  86, 'V' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  87, 'W' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  88, 'X' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  89, 'Y' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  90, 'Z' */ __CTYPE_PRINTABLE|__CTYPE_UPPER_CASE,
                /*  91, '[' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  92, '\' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  93, ']' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  94, '^' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  95, '_' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  96, '`' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /*  97, 'a' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /*  98, 'b' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /*  99, 'c' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 100, 'd' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 101, 'e' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 102, 'f' */ __CTYPE_HEX_DIGIT|__CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 103, 'g' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 104, 'h' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 105, 'i' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 106, 'j' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 107, 'k' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 108, 'l' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 109, 'm' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 110, 'n' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 111, 'o' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 112, 'p' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 113, 'q' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 114, 'r' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 115, 's' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 116, 't' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 117, 'u' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 118, 'v' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 119, 'w' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 120, 'x' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 121, 'y' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 122, 'z' */ __CTYPE_PRINTABLE|__CTYPE_LOWER_CASE,
                /* 123, '{' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /* 124, '|' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /* 125, '}' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /* 126, '~' */ __CTYPE_PUNCTUATION|__CTYPE_PRINTABLE,
                /* 127      */ __CTYPE_CONTROL,

                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
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
        /* Set memalign tree to NULL */
        .__memalign_tree = NULL,
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
    };

    if (!__clib4->__random_lock || !__clib4->__pipe_semaphore) {
        goto out;
    }

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

    /* Init memalign list */
    SHOWMSG("Allocating __memalign_pool");
    __clib4->__memalign_pool = AllocSysObjectTags(ASOT_ITEMPOOL,
                                                  ASO_NoTrack, FALSE,
                                                  ASO_MemoryOvr, MEMF_SHARED,
                                                  ASOITEM_MFlags, MEMF_SHARED,
                                                  ASOITEM_ItemSize, sizeof(struct MemalignEntry),
                                                  ASOITEM_BatchSize, 408,
                                                  ASOITEM_GCPolicy, ITEMGC_AFTERCOUNT,
                                                  ASOITEM_GCParameter, 1000,
                                                  TAG_DONE);
    if (!__clib4->__memalign_pool) {
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
        BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
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
        reent_exit(__clib4, FALSE);
    }
}

void
reent_exit(struct _clib4 *__clib4, BOOL fallback) {
    /* Free global clib structure */
    if (__clib4) {
        /* Free wchar stuff */
        if (__clib4->wide_status != NULL) {
            SHOWMSG("Freeing wide_status");
            FreeVec(__clib4->wide_status);
            __clib4->wide_status = NULL;
        }

        /* Remove random semaphore */
        SHOWMSG("Delete __random_lock semaphore");
        __delete_semaphore(__clib4->__random_lock);
        /* Remove pipe semaphore */
        SHOWMSG("Delete __pipe_semaphore semaphore");
        __delete_semaphore(__clib4->__pipe_semaphore);
        if (!fallback) { //TODO : Freeing memalign crash libExpunge and I don't know why
            /* Free memalign stuff */
            if (__clib4->__memalign_pool) {
                SHOWMSG("Freeing memalign pool");
                /* Check if we have something created with posix_memalign and not freed yet.
                 * But this is a good point also to free something allocated with memalign or
                 * aligned_alloc and all other functions are using memalign_tree to allocate memory
                 * This seems to cure also the memory leaks found sometimes (but not 100% sure..)
                 */
                struct MemalignEntry *e = (struct MemalignEntry *) AVL_FindFirstNode(__clib4->__memalign_tree);
                while (e) {
                    struct MemalignEntry *next = (struct MemalignEntry *) AVL_FindNextNodeByAddress(&e->me_AvlNode);

                    /* Free memory */
                    if (e->me_Exact) {
                        FreeVec(e->me_Exact);
                    }
                    /* Remove the node */
                    AVL_RemNodeByAddress(&__clib4->__memalign_tree, &e->me_AvlNode);
                    ItemPoolFree(__clib4->__memalign_pool, e);

                    e = next;
                }

                FreeSysObject(ASOT_ITEMPOOL, __clib4->__memalign_pool);
                SHOWMSG("Done");
            }
        }
        /* Free dl stuff */
        struct ElfIFace *IElf = __IElf;

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
    UBYTE current_dir_name[256] = {0};
    if (NameFromLock(__clib4->self->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }
}

void disableUnixPaths(void) {
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->__unix_path_semantics = FALSE;

    /* Set __current_path_name to a valid value */
    UBYTE current_dir_name[256] = {0};
    if (NameFromLock(__clib4->self->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }
}

int *__mb_cur_max(void) {
    return &__CLIB4->__mb_cur_max;
}

