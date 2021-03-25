/*
 * crtbegin.c
 *
 * :ts=4
 *
 * Handles global constructors and destructors for the OS4 GCC build.
 *
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

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>


#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */


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
 */

/****************************************************************************/
void _init(void (*__CTOR_LIST__[])(void));
void _fini(void (*__DTOR_LIST__[])(void));

extern int _main(void);
extern int _start(char *args, int arglen, struct ExecBase *sysbase);
static BOOL open_libraries(struct ExecBase *sysbase);
static void close_libraries(void);

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#include <libraries/elf.h>
#include <proto/elf.h>

struct Library  *ElfBase;
struct ElfIFace *IElf;

/****************************************************************************/

void shared_obj_init(void);
void shared_obj_exit(void);

/****************************************************************************/

static void SHLibsInit(BOOL init) {
	Elf32_Handle hSelf = (Elf32_Handle)NULL;
	BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
	if (segment_list != ZERO)
	{
		int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &hSelf, TAG_DONE);
		if (ret == 1)
		{
			if (hSelf != NULL)
			{
				Printf("Below the crash\n");
				/* Trigger the constructors, etc. in the shared objects linked to this binary. */
				InitSHLibs(hSelf, init);
				Printf("No crash on static linking\n");
			}
		}
	}
}

void shared_obj_exit(void)
{
	/* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
	SHLibsInit(FALSE);
}

void shared_obj_init()
{
	SHLibsInit(TRUE);
}

void _init(void (*__CTOR_LIST__[])(void))
{
	int num_ctors, i;
	int j;

	for (i = 1, num_ctors = 0; __CTOR_LIST__[i] != NULL; i++)
		num_ctors++;

	for (j = 0; j < num_ctors; j++)
	{
		__CTOR_LIST__[num_ctors - j]();
	}
}

void _fini(void (*__DTOR_LIST__[])(void))
{
	int num_dtors, i;
	static int j;
	extern void shared_obj_exit(void);

	for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
		num_dtors++;

	while (j++ < num_dtors)
		__DTOR_LIST__[j]();

	/* The shared objects need to be cleaned up after all local
	   destructors have been invoked. */
	shared_obj_exit();
}

int 
_start(char *args, int arglen, struct ExecBase *sysbase)
{
	int result = -1;
	extern void shared_obj_init(void);

	/* Try to open the libraries we need to proceed. */
	if (CANNOT open_libraries(sysbase))
	{
		const char *error_message;

		/* If available, use the error message provided by the client. */
		error_message = __minimum_os_lib_error;

		if (error_message == NULL)
			error_message = "This program requires AmigaOS 4.1 or higher.";

		__show_error(error_message);
		goto out;
	}

	/* The shared objects need to be set up before any local
	   constructors are invoked. */
	shared_obj_init();

   	result = _main();

out:
	close_libraries();
   	return result;
}

static BOOL 
open_libraries(struct ExecBase *sysbase)
{
	BOOL success = FALSE;

	/* Get exec interface */
	IExec = (struct ExecIFace *)((struct ExecBase *)sysbase)->MainInterface;

	/* Open the minimum required libraries. */
	DOSBase = (struct Library *)OpenLibrary("dos.library", 54);
	if (DOSBase == NULL)
		goto out;

	__UtilityBase = OpenLibrary("utility.library", 54);
	if (__UtilityBase == NULL)
		goto out;

	/* Obtain the interfaces for these libraries. */
	IDOS = (struct DOSIFace *)GetInterface(DOSBase, "main", 1, 0);
	if (IDOS == NULL)
		goto out;

	__IUtility = (struct UtilityIFace *)GetInterface(__UtilityBase, "main", 1, 0);
	if (__IUtility == NULL)
		goto out;

	/* We need elf.library V52.2 or higher. */
	ElfBase = OpenLibrary("elf.library", 0);
	if (ElfBase == NULL || (ElfBase->lib_Version < 52) || (ElfBase->lib_Version == 52 && ElfBase->lib_Revision < 2))
		goto out;

	IElf = (struct ElfIFace *)GetInterface(ElfBase, "main", 1, NULL);
	if (IElf == NULL)
		goto out;

	success = TRUE;

out:

	return (success);
}

static void close_libraries(void) {

	if (IDOS != NULL)
	{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
	}

	if (DOSBase != NULL)
	{
		CloseLibrary(DOSBase);
		DOSBase = NULL;
	}

	if (IElf != NULL)
	{
		DropInterface((struct Interface *)IElf);
		IElf = NULL;
	}

	if (ElfBase != NULL)
	{
		CloseLibrary(ElfBase);
		ElfBase = NULL;
	}
}