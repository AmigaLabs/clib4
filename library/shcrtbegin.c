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

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timerequest TimeRequest
#define tr_node Request

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */


/* Avoid gcc warnings.. */
void __shlib_call_constructors(void);
void __shlib_call_destructors(void);

extern struct ExecIFace NOCOMMON *IExec;
extern struct Library NOCOMMON *__ElfBase;
extern struct ElfIFace NOCOMMON *__IElf;

/* Local timer I/O. */
extern struct MsgPort *NOCOMMON __timer_port;
extern struct timerequest *NOCOMMON __timer_request;

extern struct Library *NOCOMMON __TimerBase;
extern struct TimerIFace *NOCOMMON __ITimer;

/****************************************************************************/
/*
CLIB_CONSTRUCTOR(timer_init)
{
	BOOL success = FALSE;

	ENTER();

	__timer_port = AllocSysObjectTags(ASOT_PORT, ASOPORT_AllocSig, FALSE, ASOPORT_Signal, SIGB_SINGLE, TAG_DONE);
	if (__timer_port == NULL)
	{
		__show_error("The timer message port could not be created.");
		goto out;
	}
	
	__timer_request = AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct TimeRequest), ASOMSG_ReplyPort, __timer_port, TAG_DONE);
	if (__timer_request == NULL)
	{
		__show_error("The timer I/O request could not be created.");
		goto out;
	}

	if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)__timer_request, 0) != OK)
	{
		__show_error("The timer could not be opened.");
		goto out;
	}

	__TimerBase = (struct Library *)__timer_request->tr_node.io_Device;
	__ITimer = (struct TimerIFace *)GetInterface(__TimerBase, "main", 1, 0);
	if (__ITimer == NULL)
	{
		__show_error("The timer interface could not be obtained.");
		goto out;
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


CLIB_DESTRUCTOR(timer_exit)
{
	ENTER();

	if (__ITimer != NULL)
		DropInterface((struct Interface *)__ITimer);

	__ITimer = NULL;

	__TimerBase = NULL;

	if (__timer_request != NULL)
	{
		if (__timer_request->tr_node.io_Device != NULL)
			CloseDevice((struct IORequest *)__timer_request);

		FreeSysObject(ASOT_MESSAGE, __timer_request);
		__timer_request = NULL;
	}

	if (__timer_port != NULL)
	{
		FreeSysObject(ASOT_PORT, __timer_port);
		__timer_port = NULL;
	}

	LEAVE();
}
*/
static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"), aligned(sizeof(void (*)(void)))));
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"), aligned(sizeof(void (*)(void)))));

void 
__shlib_call_constructors(void)
{
   int num_ctors, i;
   int j;

   SysBase = *(struct Library **)4;
   IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
  
   /* The libraries needs to be set up before any local constructors are invoked. */

   for (i = 1, num_ctors = 0; __CTOR_LIST__[i] != NULL; i++)
      num_ctors++;

   for (j = 0; j < num_ctors; j++)
      __CTOR_LIST__[num_ctors - j]();
}

void 
__shlib_call_destructors(void)
{
   int num_dtors, i;
   static int j = 0;

   for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
      num_dtors++;

   while (j++ < num_dtors)
      __DTOR_LIST__[j]();

   /* The libraries needs to be cleaned up after all local destructors have been invoked. */
   //close_libraries();
}
