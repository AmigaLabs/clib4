/*
 * $Id: shcrtbegin.c,v 1.0 2021-02-01 17:22:03 apalmate Exp $
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

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"), aligned(sizeof(void (*)(void)))));
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"), aligned(sizeof(void (*)(void)))));

/* Avoid gcc warnings.. */
void __shlib_call_constructors(void);
void __shlib_call_destructors(void);

extern struct ExecIFace NOCOMMON *IExec;
extern struct Library NOCOMMON *__ElfBase;
extern struct ElfIFace NOCOMMON *__IElf;

STATIC BOOL
open_libraries(struct ExecIFace *iexec)
{
   BOOL success = FALSE;

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
   __ElfBase = OpenLibrary("elf.library", 0);
   if (__ElfBase == NULL || (__ElfBase->lib_Version < 52) || (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision < 2))
      goto out;

   __IElf = (struct ElfIFace *)GetInterface(__ElfBase, "main", 1, NULL);
   if (__IElf == NULL)
      goto out;

   success = TRUE;

out:

   return (success);
}

STATIC VOID
close_libraries(VOID)
{
   if (__IUtility != NULL)
   {
      DropInterface((struct Interface *)__IUtility);
      __IUtility = NULL;
   }

   if (IDOS != NULL)
   {
      DropInterface((struct Interface *)IDOS);
      IDOS = NULL;
   }

   if (__UtilityBase != NULL)
   {
      CloseLibrary(__UtilityBase);
      __UtilityBase = NULL;
   }

   if (DOSBase != NULL)
   {
      CloseLibrary(DOSBase);
      DOSBase = NULL;
   }

   if (__IElf != NULL)
   {
      DropInterface((struct Interface *)__IElf);
      __IElf = NULL;
   }

   if (__ElfBase != NULL)
   {
      CloseLibrary(__ElfBase);
      __ElfBase = NULL;
   }
}

void 
__shlib_call_constructors(void)
{
   int num_ctors, i;
   int j;

   SysBase = *(struct Library **)4;
   IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;

   /* The libraries needs to be set up before any local constructors are invoked. */
   open_libraries(IExec);

   for (i = 1, num_ctors = 0; __CTOR_LIST__[i] != NULL; i++)
      num_ctors++;

   for (j = 0; j < num_ctors; j++)
      __CTOR_LIST__[num_ctors - j]();
   
   // Create global clib structure

   /*
	__global_clib2 = InitGlobal();
	if (__global_clib2 == NULL)
		abort();
      */
}

void 
__shlib_call_destructors(void)
{
   int num_dtors, i;
   static int j;

   /* Free global clib structure */
   //FiniGlobal();

   for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
      num_dtors++;

   while (j++ < num_dtors)
      __DTOR_LIST__[j]();

   /* The libraries needs to be cleaned up after all local destructors have been invoked. */
   close_libraries();
}
