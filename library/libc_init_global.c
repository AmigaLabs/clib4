/*
 * $Id: libc_init_global.c,v 1.0 2021-02-04 17:01:06 clib2devs Exp $
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
reent_init(struct _clib2 *__clib2) {
    BOOL success = FALSE;

    ENTER();
    DECLARE_UTILITYBASE();

    struct ElfIFace *IElf = __IElf;
    /* Set main Exec and IElf interface pointers */
    __clib2->IExec = IExec;
    __clib2->IElf = __IElf;

    /* Enable check abort */
    __clib2->__check_abort_enabled = TRUE;

    /* Get cpu family used to choose functions at runtime */
    D(("Setting cpu family"));
    GetCPUInfoTags(GCIT_Family, &__clib2->cpufamily, TAG_DONE);

    /* Check if altivec is present */
#ifdef ENABLE_ALTIVEC_AT_START
    D(("Check if altivec is present"));
    GetCPUInfoTags(GCIT_VectorUnit, &__clib2->hasAltivec, TAG_DONE);
#else
    D(("Set altivec to zero"));
    __clib2->hasAltivec = 0;
#endif

    __clib2->__break_signal_mask = SIGBREAKF_CTRL_C;

    D(("Setting global errno"));
    __clib2->_errno = 0;

    __clib2->__shell_escape_character = '*';
    __clib2->__cache_line_size = 32;
    /* We use an invalid priority value to indicate "do not change the program's task priority". */
    __clib2->__priority = 256;

    SHOWMSG("Allocating wide_status");
    /* Initialize wchar stuff */
    __clib2->wide_status = AllocVecTags(sizeof(struct _wchar), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (!__clib2->wide_status) {
        goto out;
    }

#ifdef DISABLE_OPTIMIZED_FUNCTIONS_AT_START
    __clib2->__optimizedCPUFunctions = FALSE;
#else
    __clib2->__optimizedCPUFunctions = TRUE;
#endif

    /* Init memalign list */
    SHOWMSG("Allocating __memalign_pool");
    __clib2->__memalign_pool = AllocSysObjectTags(ASOT_ITEMPOOL,
                                                  ASO_NoTrack, FALSE,
                                                  ASO_MemoryOvr, MEMF_PRIVATE,
                                                  ASOITEM_MFlags, MEMF_PRIVATE,
                                                  ASOITEM_ItemSize, sizeof(struct MemalignEntry),
                                                  ASOITEM_BatchSize, 408,
                                                  ASOITEM_GCPolicy, ITEMGC_AFTERCOUNT,
                                                  ASOITEM_GCParameter, 1000,
                                                  TAG_DONE);
    if (!__clib2->__memalign_pool) {
        goto out;
    }
    /* Set memalign tree to NULL */
    __clib2->__memalign_tree = NULL;

    /* Initialize random signal and state */
    __clib2->__random_lock = __create_semaphore();
    if (!__clib2->__random_lock) {
        goto out;
    }
    __clib2->n = 31;
    __clib2->i = 3;
    __clib2->j = 0;
    __clib2->x = _random_init + 1;

    __clib2->_gamma_signgam = 0;
    __clib2->__infinity = 0;
    __clib2->__nan = 0;
    /*
     * Next: Get Elf handle associated with the currently running process.
     * ElfBase is opened in crtbegin.c that is called before the
     * call_main()
     */

    SHOWMSG("Try to get elf handle for dl* operations");
    if (__clib2->IElf != NULL) {
        SHOWMSG("Calling GetProcSegList");
        BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
        if (segment_list != ZERO) {
            Elf32_Handle handle = NULL;

            SHOWMSG("Calling GetSegListInfoTags");
            if (GetSegListInfoTags(segment_list, GSLI_ElfHandle, &handle, TAG_DONE) == 1) {
                if (handle != NULL) {
                    SHOWMSG("Calling OpenElfTags");
                    __clib2->__dl_elf_handle = OpenElfTags(OET_ElfHandle, handle, OET_ReadOnlyCopy, TRUE, TAG_DONE);
                    SHOWPOINTER(__clib2->__dl_elf_handle);
                }
            }
        }
    }

    __clib2->wide_status->_strtok_last = NULL;
    __clib2->wide_status->_mblen_state.__count = 0;
    __clib2->wide_status->_mblen_state.__value.__wch = 0;
    __clib2->wide_status->_wctomb_state.__count = 0;
    __clib2->wide_status->_wctomb_state.__value.__wch = 0;
    __clib2->wide_status->_mbtowc_state.__count = 0;
    __clib2->wide_status->_mbtowc_state.__value.__wch = 0;
    __clib2->wide_status->_mbrlen_state.__count = 0;
    __clib2->wide_status->_mbrlen_state.__value.__wch = 0;
    __clib2->wide_status->_mbrtowc_state.__count = 0;
    __clib2->wide_status->_mbrtowc_state.__value.__wch = 0;
    __clib2->wide_status->_mbsrtowcs_state.__count = 0;
    __clib2->wide_status->_mbsrtowcs_state.__value.__wch = 0;
    __clib2->wide_status->_wcrtomb_state.__count = 0;
    __clib2->wide_status->_wcrtomb_state.__value.__wch = 0;
    __clib2->wide_status->_wcsrtombs_state.__count = 0;
    __clib2->wide_status->_wcsrtombs_state.__value.__wch = 0;
    __clib2->wide_status->_l64a_buf[0] = '\0';
    __clib2->wide_status->_getdate_err = 0;
    /* Set locale stuff */
    __clib2->_current_category = LC_ALL;
    __clib2->_current_locale = "C-UTF-8";
    __clib2->__mb_cur_max = 1;

    /* Check is SYSV library is available in the system */
    __clib2->haveShm = FALSE;
    SHOWMSG("try to open SYSVIPC Library");
    __clib2->__SysVBase = OpenLibrary("sysvipc.library", 53);
    if (__clib2->__SysVBase != NULL) {
        __clib2->__ISysVIPC = (struct SYSVIFace *) GetInterface(__clib2->__SysVBase, "main", 1, NULL);
        if (__clib2->__ISysVIPC != NULL) {
            __clib2->haveShm = TRUE;
        } else {
            CloseLibrary(__clib2->__SysVBase);
            __clib2->__SysVBase = NULL;
        }
    }

    __clib2->__disable_dos_requesters = FALSE;
    __clib2->__expand_wildcard_args = TRUE;
    __clib2->__unlink_retries = TRUE;

    /* Clear itimer start time */
    __clib2->tmr_start_time.tv_sec = 0;
    __clib2->tmr_start_time.tv_usec = 0;
    __clib2->tmr_real_task = NULL;
    ClearMem(&__clib2->tmr_time, sizeof(struct itimerval));

    __clib2->__default_path_delimiter = ":";
    __clib2->__default_path = "/C:.:/APPDIR:/PROGDIR:/ram:/SDK/C:/SDK/Local/C:";

    /* Check if .unix file exists in the current dir. If the file exists enable unix path semantics */
    SHOWMSG("Check for .unix file");
    __clib2->__unix_path_semantics = FALSE;
    struct ExamineData *exd = ExamineObjectTags(EX_StringNameInput, (CONST_STRPTR) ".unix", TAG_DONE);
    if (exd != NULL) {
        if (EXD_IS_FILE(exd)) {
            SHOWMSG("Enable unix paths");
            __clib2->__unix_path_semantics = TRUE;
        }
        FreeDosObject(DOS_EXAMINEDATA, exd);
        exd = NULL;
    }

    /* This table holds pointers to all signal handlers configured at a time. */
    SHOWMSG("Set signal table to SIG_DFL");
    for (int i = 0; i < NSIG; i++) {
        __clib2->__signal_handler_table[i] = SIG_DFL;
    }

    /* Default root groups and id */
    __clib2->__root_mode = FALSE;
    __clib2->__root_uid = 0;
    __clib2->__root_gid = 0;
    __clib2->__root_euid = 0;
    __clib2->__root_egid = 0;

    success = TRUE;

out:

    if (!success) {
       reent_exit(__clib2);
    }
}

void
reent_exit(struct _clib2 *__clib2) {
    ENTER();

    /* Free global clib structure */
    if (__clib2) {
        struct ElfIFace *IElf = __IElf;

        /* Free wchar stuff */
        if (__clib2->wide_status != NULL) {
            SHOWMSG("Free wide_status");
            FreeVec(__clib2->wide_status);
            __clib2->wide_status = NULL;
        }
        /* Remove random semaphore */
        SHOWMSG("Delete random lock semaphore");
        __delete_semaphore(__clib2->__random_lock);

        if (__clib2->__ISysVIPC != NULL) {
            SHOWMSG("Drop SYSV interface");
            DropInterface((struct Interface *) __clib2->__ISysVIPC);
            __clib2->__ISysVIPC = NULL;
        }

        if (__clib2->__SysVBase != NULL) {
            SHOWMSG("Close SYSV Library");
            CloseLibrary(__clib2->__SysVBase);
            __clib2->__SysVBase = NULL;
        }

        /* Free memalign stuff */
        if (__clib2->__memalign_pool) {
            SHOWMSG("Clean memalign memory");
            /* Check if we have something created with posix_memalign and not freed yet.
             * But this is a good point also to free something allocated with memalign or
             * aligned_alloc and all other functions are using memalign_tree to allocate memory
             * This seems to cure also the memory leaks found sometimes (but not 100% sure..)
             */
            struct MemalignEntry *e = (struct MemalignEntry *) AVL_FindFirstNode(__clib2->__memalign_tree);
            while (e) {
                struct MemalignEntry *next = (struct MemalignEntry *) AVL_FindNextNodeByAddress(&e->me_AvlNode);

                /* Free memory */
                if (e->me_Exact) {
                    FreeVec(e->me_Exact);
                }
                /* Remove the node */
                AVL_RemNodeByAddress(&__clib2->__memalign_tree, &e->me_AvlNode);
                ItemPoolFree(__clib2->__memalign_pool, e);

                e = next;
            }

            FreeSysObject(ASOT_ITEMPOOL, __clib2->__memalign_pool);
        }

        /* Free dl stuff */
        if (__clib2->__dl_elf_handle != NULL) {
            SHOWMSG("Closing elf handle");
            CloseElfTags(__clib2->__dl_elf_handle, CET_ReClose, TRUE, TAG_DONE);
            __clib2->__dl_elf_handle = NULL;
        }
        else {
            D(("Cannot close elf handle: __clib2->__dl_elf_handle == %p - IElf == %p", __clib2->__dl_elf_handle, IElf));
        }

        FreeVec(__clib2);
        __clib2 = NULL;
    }

    LEAVE();
}

void enableUnixPaths(void) {
    __CLIB2->__unix_path_semantics = TRUE;
}

void disableUnixPaths(void) {
    __CLIB2->__unix_path_semantics = FALSE;
}

void enableAltivec(void) {
    int32 hasAltivec;
    /* Check if altivec is present otherwise we can't enable it */
    GetCPUInfoTags(GCIT_VectorUnit, &hasAltivec, TAG_DONE);
    if (hasAltivec)
        __CLIB2->hasAltivec = 1;
    else
        __CLIB2->hasAltivec = 0;
}

void disableAltivec(void) {
    __CLIB2->hasAltivec = 0;
}

void enableOptimizedFunctions(void) {
    __CLIB2->__optimizedCPUFunctions = TRUE;
};

void disableOptimizedFunctions(void) {
    __CLIB2->__optimizedCPUFunctions = FALSE;
};

int *__mb_cur_max(void) {
    return &__CLIB2->__mb_cur_max;
}

