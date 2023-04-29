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

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
static struct Library *__ElfBase;
static struct ElfIFace *__IElf;

extern struct _clib2 *__clib2;

void
reent_init() {
    BOOL success = FALSE;

    ENTER();

    /* We need elf.library V52.2 or higher. */
    __ElfBase = OpenLibrary("elf.library", 0);
    if (__ElfBase == NULL || (__ElfBase->lib_Version < 52) || (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision < 2)) {
        SHOWMSG("Cannot get __ElfBase >=52.2!");
        goto out;
    }

    __IElf = (struct ElfIFace *) GetInterface(__ElfBase, "main", 1, NULL);
    if (__IElf == NULL) {
        SHOWMSG("Cannot get __IElf!");
        goto out;
    }

    /* Initialize global structure */
    __clib2 = (struct _clib2 *) AllocVecTags(sizeof(struct _clib2), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
    if (__clib2 == NULL) {
        goto out;
    } else {
        struct ElfIFace *IElf = __IElf;

        /* Get the current task pointer */
        __clib2->self = (struct Process *) FindTask(NULL);

        /* Initialize wchar stuff */
        __clib2->wide_status = AllocVecTags(sizeof(struct _wchar), AVT_Type, MEMF_SHARED, TAG_DONE);
        if (!__clib2->wide_status) {
            FreeVec(__clib2);
            __clib2 = NULL;
            goto out;
        }

#ifdef DISABLE_OPTIMIZED_FUNCTIONS_AT_START
        __CLIB2->__optimizedCPUFunctions = FALSE;
#else
        __CLIB2->__optimizedCPUFunctions = TRUE;
#endif

        /* Initialize random signal and state */
        __clib2->__random_lock = __create_semaphore();
        if (!__clib2->__random_lock) {
            FreeVec(__clib2->wide_status);
            FreeVec(__clib2);
            __clib2 = NULL;
            goto out;
        }
        __clib2->n = 31;
        __clib2->i = 3;
        __clib2->j = 0;
        __clib2->x = _random_init + 1;

        /* Set main Exec and IElf interface pointers */
        __clib2->IExec = IExec;
        __clib2->IElf = __IElf;

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
        __SysVBase = OpenLibrary("sysvipc.library", 53);
        if (__SysVBase != NULL) {
            __ISysVIPC = (struct SYSVIFace *) GetInterface(__SysVBase, "main", 1, NULL);
            if (__ISysVIPC != NULL) {
                __clib2->haveShm = TRUE;
            } else {
                CloseLibrary(__SysVBase);
                __SysVBase = NULL;
            }
        }

        /* Clear itimer start time */
        __clib2->tmr_start_time.tv_sec = 0;
        __clib2->tmr_start_time.tv_usec = 0;
        __clib2->tmr_real_task = NULL;
    }
    success = TRUE;

out:

    if (!success) {
        /* Clean wide status memory */
        if (__clib2->wide_status) {
            FreeVec(__clib2->wide_status);
            __clib2->wide_status = NULL;
        }

        /* Free library */
        if (__clib2) {
            FreeVec(__clib2);
            __clib2 = NULL;
        }

        if (__IElf != NULL) {
            DropInterface((struct Interface *) __IElf);
            __IElf = NULL;
        }

        if (__ElfBase != NULL) {
            CloseLibrary(__ElfBase);
            __ElfBase = NULL;
        }
    }
}

void
reent_exit() {
    ENTER();

    struct ElfIFace *IElf = __IElf;

    /* Free global clib structure */
    if (__clib2) {
        /* Free wchar stuff */
        if (__clib2->wide_status != NULL) {
            FreeVec(__clib2->wide_status);
            __clib2->wide_status = NULL;
        }
        /* Remove random semaphore */
        __delete_semaphore(__clib2->__random_lock);

        if (__ISysVIPC != NULL) {
            DropInterface((struct Interface *) __ISysVIPC);
            __ISysVIPC = NULL;
        }

        if (__SysVBase != NULL) {
            CloseLibrary(__SysVBase);
            __SysVBase = NULL;
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
    GetCPUInfoTags(GCIT_VectorUnit, &hasAltivec);
    if (hasAltivec)
        __GCLIB2->hasAltivec = 1;
    else
        __GCLIB2->hasAltivec = 0;
}

void disableAltivec(void) {
    __GCLIB2->hasAltivec = 0;
}

void enableOptimizedFunctions(void) {
    __CLIB2->__optimizedCPUFunctions = TRUE;
};

void disableOptimizedFunctions(void) {
    __CLIB2->__optimizedCPUFunctions = FALSE;
};

int *__mb_cur_max(void) {
    return &__clib2->__mb_cur_max;
}

