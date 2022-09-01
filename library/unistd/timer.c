/*
 * $Id: unistd_timer.c,v 1.11 2021-02-01 16:35:56 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timerequest TimeRequest
#define tr_node Request

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

/****************************************************************************/

/* Local timer I/O. */
struct MsgPort *NOCOMMON __timer_port;
struct timerequest *NOCOMMON __timer_request;
BOOL NOCOMMON __timer_busy;

struct Library *NOCOMMON __TimerBase;
struct TimerIFace *NOCOMMON __ITimer;

/****************************************************************************/

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

    /* Set system time for rusage */
    struct TimerIFace *ITimer = __ITimer;
    GetSysTime(&__global_clib2->clock);
    /* Generate random seed */
    __global_clib2->__random_seed = time(NULL);


success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}

/****************************************************************************/

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
