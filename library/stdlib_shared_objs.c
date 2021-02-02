/*
 * $Id: stdlib_shared_objs.c,v 1.1 2010-08-21 11:37:03 obarthel Exp $
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
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#include <libraries/elf.h>
#include <proto/elf.h>

/****************************************************************************/

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
struct Library  NOCOMMON *__ElfBase;
struct ElfIFace NOCOMMON *__IElf;

/****************************************************************************/

/* This is used to initialize the shared objects only. */
static Elf32_Handle elf_handle;

/****************************************************************************/

void shared_obj_init(struct ExecIFace *iexec);
void shared_obj_exit(void);
extern BOOL open_libraries(struct ExecIFace *iexec);

/****************************************************************************/

void shared_obj_exit(void)
{
	struct ElfIFace *IElf = __IElf;

	/* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
	if (elf_handle != NULL)
	{
		InitSHLibs(elf_handle, FALSE);
		elf_handle = NULL;
	}
}

/****************************************************************************/

void shared_obj_init(struct ExecIFace *iexec)
{
	/* 
	 * if IExec is null this means we are loaded as shared object file.
	 * We need to init all stuff we need to work correctly
	 */
	if (open_libraries(iexec)) {
		struct ElfIFace *IElf = __IElf;
		BPTR segment_list;

		/* Try to find the Elf handle associated with this program's segment list. */
		struct Process *self = (struct Process *) FindTask(0);
		segment_list = GetProcSegList(self, GPSLF_CLI | GPSLF_SEG);
		if (segment_list != ZERO)
		{
			int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &elf_handle, TAG_DONE);
			if (ret == 1)
			{
				if (elf_handle != NULL)
				{
					/* Trigger the constructors, etc. in the shared objects linked to this binary. */
					InitSHLibs(elf_handle, TRUE);
				}
			}
		}
	}
}

