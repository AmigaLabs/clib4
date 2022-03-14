/*
 * $Id: libc_init_global.c,v 1.0 2021-02-04 17:01:06 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
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

#include <proto/elf.h>
#include <fenv.h>

/* random table */
static uint32_t _random_init[] = {
        0x00000000,0x5851f42d,0xc0b18ccf,0xcbb5f646,
        0xc7033129,0x30705b04,0x20fd5db4,0x9a8b7f78,
        0x502959d8,0xab894868,0x6c0356a7,0x88cdb7ff,
        0xb477d43f,0x70a3a52b,0xa8e4baf1,0xfd8341fc,
        0x8ae16fd9,0x742d2f7a,0x0d1f0796,0x76035e09,
        0x40f7702c,0x6fa72ca5,0xaaa84157,0x58a0df74,
        0xc74a0364,0xae533cc4,0x04185faf,0x6de3b115,
        0x0cab8628,0xf043bfa4,0x398150e9,0x37521657};

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
extern struct Library  *__ElfBase;
extern struct ElfIFace *__IElf;

struct _clib2 NOCOMMON*__global_clib2;

STDLIB_CONSTRUCTOR(global_init)
{
    BOOL success = FALSE;

    ENTER();

/* Initialize global structure */
	__global_clib2 = (struct _clib2 *)AllocVecTags(sizeof(struct _clib2), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
	if (__global_clib2 == NULL)
	{
		goto out;
	}
	else
	{
		struct ElfIFace *IElf = __IElf;

		/* Initialize wchar stuff */
		__global_clib2->wide_status = AllocVecTags(sizeof(struct _wchar), AVT_Type, MEMF_SHARED, TAG_DONE);
		if (!__global_clib2->wide_status)
		{
			FreeVec(__global_clib2);
			__global_clib2 = NULL;
			goto out;
		}

        /* Initialize random signal and state */
        __global_clib2->__random_lock = __create_semaphore();
        if (!__global_clib2->__random_lock) {
            FreeVec(__global_clib2->wide_status);
            FreeVec(__global_clib2);
            __global_clib2 = NULL;
            goto out;
        }
        __global_clib2->n = 31;
        __global_clib2->i = 3;
        __global_clib2->j = 0;
        __global_clib2->x = _random_init + 1;

		/* Set main Exec and IElf interface pointers */
		__global_clib2->IExec = IExec;
		__global_clib2->IElf = __IElf;

		__global_clib2->wide_status->_strtok_last = NULL;
		__global_clib2->wide_status->_mblen_state.__count = 0;
		__global_clib2->wide_status->_mblen_state.__value.__wch = 0;
		__global_clib2->wide_status->_wctomb_state.__count = 0;
		__global_clib2->wide_status->_wctomb_state.__value.__wch = 0;
		__global_clib2->wide_status->_mbtowc_state.__count = 0;
		__global_clib2->wide_status->_mbtowc_state.__value.__wch = 0;
		__global_clib2->wide_status->_mbrlen_state.__count = 0;
		__global_clib2->wide_status->_mbrlen_state.__value.__wch = 0;
		__global_clib2->wide_status->_mbrtowc_state.__count = 0;
		__global_clib2->wide_status->_mbrtowc_state.__value.__wch = 0;
		__global_clib2->wide_status->_mbsrtowcs_state.__count = 0;
		__global_clib2->wide_status->_mbsrtowcs_state.__value.__wch = 0;
		__global_clib2->wide_status->_wcrtomb_state.__count = 0;
		__global_clib2->wide_status->_wcrtomb_state.__value.__wch = 0;
		__global_clib2->wide_status->_wcsrtombs_state.__count = 0;
		__global_clib2->wide_status->_wcsrtombs_state.__value.__wch = 0;
		__global_clib2->wide_status->_l64a_buf[0] = '\0';
		__global_clib2->wide_status->_getdate_err = 0;
        /* Set locale stuff */
        __global_clib2->_current_category = LC_ALL;
        __global_clib2->_current_locale = "C-UTF-8";

		/* Get the current task pointer */
		__global_clib2->self = (struct Process *)FindTask(0);

        /* clear tempnam stuff */
		srand(time(NULL));
		__global_clib2->inc = 0;
		memset(__global_clib2->emergency, 0, sizeof(__global_clib2->emergency));

        /* Clear fenv flags */
        feclearexcept(FE_ALL_EXCEPT);

		/* Init memalign list */
        __global_clib2->__memalign_pool = AllocSysObjectTags(ASOT_ITEMPOOL,
                                                            ASO_NoTrack,         FALSE,
                                                            ASO_MemoryOvr,       MEMF_PRIVATE,
                                                            ASOITEM_MFlags,      MEMF_PRIVATE,
                                                            ASOITEM_ItemSize ,   sizeof(struct MemalignEntry),
                                                            ASOITEM_BatchSize,   408,
                                                            ASOITEM_GCPolicy,    ITEMGC_AFTERCOUNT,
                                                            ASOITEM_GCParameter, 1000,
                                                            TAG_DONE);

		/* Check is SYSV library is available in the system */
		__global_clib2->haveShm = FALSE;
		__SysVBase = OpenLibrary("sysvipc.library", 53);
		if (__SysVBase != NULL)
		{
			__ISysVIPC = (struct SYSVIFace *)GetInterface(__SysVBase, "main", 1, NULL);
			if (__ISysVIPC != NULL)
			{
				__global_clib2->haveShm = TRUE;
			}
			else
			{
				CloseLibrary(__SysVBase);
				__SysVBase = NULL;
			}
		}

		/* Check if .unix file exists in the current dir. If the file exists enable 
		 * unix path semantics
		 */
		__global_clib2->__unix_path_semantics = FALSE;
		struct ExamineData *exd = ExamineObjectTags(EX_StringNameInput, (CONST_STRPTR) ".unix", TAG_DONE);
		if (exd != NULL)
		{
			if (EXD_IS_FILE(exd))
				__global_clib2->__unix_path_semantics = TRUE;
			FreeDosObject(DOS_EXAMINEDATA, exd);
		}

		/* Choose which memcpy to use */
		GetCPUInfoTags(GCIT_Family, &__global_clib2->cpufamily);

		/*
		 * Next: Get Elf handle associated with the currently running process.
		 * ElfBase is opened in crtbegin.c that is called before the
		 * call_main()
		 */

		if (__ElfBase != NULL)
		{
			BPTR segment_list = GetProcSegList(NULL, GPSLF_CLI | GPSLF_SEG);
			if (segment_list != ZERO)
			{
				Elf32_Handle handle = NULL;

				if (GetSegListInfoTags(segment_list, GSLI_ElfHandle, &handle, TAG_DONE) == 1)
				{
					if (handle != NULL)
					{
						__global_clib2->__dl_elf_handle = OpenElfTags(OET_ElfHandle, handle, TAG_DONE);
					}
				}
			}
		}
	}
    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}

STDLIB_DESTRUCTOR(global_exit)
{
    ENTER();

	struct ElfIFace *IElf = __IElf;

    /* Free global clib structure */
	if (__global_clib2)
	{
        /* Free memalign stuff */
		if (&__global_clib2->__memalign_pool)
		{
            FreeSysObject(ASOT_ITEMPOOL, __global_clib2->__memalign_pool);
		}

		if (__ISysVIPC != NULL)
		{
			DropInterface((struct Interface *)__ISysVIPC);
			__ISysVIPC = NULL;
		}

		if (__SysVBase != NULL)
		{
			CloseLibrary(__SysVBase);
			__SysVBase = NULL;
		}

		/* Free wchar stuff */
		if (__global_clib2->wide_status != NULL)
		{
			FreeVec(__global_clib2->wide_status);
			__global_clib2->wide_status = NULL;
		}

		/* Free dl stuff */
		if (__global_clib2->__dl_elf_handle != NULL)
		{
			CloseElfTags(__global_clib2->__dl_elf_handle, CET_ReClose, TRUE, TAG_DONE);
			__global_clib2->__dl_elf_handle = NULL;
		}

        /* Remove random semaphore */
        __delete_semaphore(__global_clib2->__random_lock);

		FreeVec(__global_clib2);
		__global_clib2 = NULL;
	}

    LEAVE();
}

void enableUnixPaths(void)
{
    __global_clib2->__unix_path_semantics = TRUE;
}

void disableUnixPaths(void)
{
    __global_clib2->__unix_path_semantics = FALSE;
}

int *__mb_cur_max(void)
{
    return &__global_clib2->__mb_cur_max;
}