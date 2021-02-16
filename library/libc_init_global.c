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

#include <proto/elf.h>

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
extern struct Library  NOCOMMON *__ElfBase;
extern struct ElfIFace NOCOMMON *__IElf;

struct _clib2 * InitGlobal() {
    /* Initialize global structure */
	__global_clib2 = (struct _clib2 *)AllocVecTags(sizeof(struct _clib2), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
	if (__global_clib2 == NULL)
	{
		goto out;
	}
	else
	{
		struct TimerIFace *ITimer = __ITimer;
        struct ElfIFace *IElf = __IElf;

		/* Initialize wchar stuff */
		__global_clib2->wide_status = AllocVecTags(sizeof(struct _wchar), AVT_Type, MEMF_SHARED, TAG_DONE);
		if (!__global_clib2->wide_status) {
            FreeVec(__global_clib2);
            __global_clib2 = NULL;
			goto out;
		}
		/* Set main Exec interface pointer */
		__global_clib2->IExec = IExec;

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
		
		/* Get the current task pointer */
		__global_clib2->self = (struct Process *)FindTask(0);

		/* Set system time for rusage */
		GetSysTime(&__global_clib2->clock);

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
		if (exd != NULL) {
			if (EXD_IS_FILE(exd))
				__global_clib2->__unix_path_semantics = TRUE;
			FreeDosObject(DOS_EXAMINEDATA, exd);
		}

		/* 
		 * Next: Get Elf handle associated with the currently running process. 
		 * __ElfBase is opened in stdlib_shared_objs.c that is called before the
		 * call_main()
		 */

		if (__ElfBase != NULL)
		{
			BPTR segment_list = GetProcSegList(NULL,  GPSLF_RUN | GPSLF_SEG);
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
out:
    return __global_clib2;
}

void FiniGlobal() {
    struct ElfIFace *IElf = __IElf;
    
    /* Free global clib structure */
	if (__global_clib2)
	{
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
		if (__global_clib2->wide_status != NULL) {
			FreeVec(__global_clib2->wide_status);
            __global_clib2->wide_status = NULL;
        }

		/* Free dl stuff */
		if (__global_clib2->__dl_elf_handle != NULL)
		{
			CloseElfTags(__global_clib2->__dl_elf_handle, CET_ReClose, TRUE, TAG_DONE);
			__global_clib2->__dl_elf_handle = NULL;
		}

		FreeVec(__global_clib2);
	}
}