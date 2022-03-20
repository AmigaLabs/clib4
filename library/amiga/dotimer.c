/*
 * $Id: amiga_dotimer.c,v 1.9 2021-01-31 06:43:56 clib2devs Exp $
*/

#include <exec/errors.h>
#include <devices/timer.h>

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "macros.h"
#include "debug.h"

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timeval TimeVal
#define tv_secs Seconds
#define tv_micro Microseconds

#define timerequest TimeRequest
#define tr_node Request
#define tr_time Time

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

/****************************************************************************/

LONG DoTimer(struct timeval *tv, LONG unit, LONG command)
{
	struct TimeRequest *tr = NULL;
	struct MsgPort *mp;
	LONG error;

	assert(tv != NULL);

	mp = AllocSysObjectTags(ASOT_PORT,
							ASOPORT_Action, PA_SIGNAL,
							ASOPORT_AllocSig, FALSE,
							ASOPORT_Signal, SIGB_SINGLE,
							ASOPORT_Target, FindTask(NULL),
							TAG_DONE);

	if (mp == NULL)
	{
		error = IOERR_OPENFAIL;
		goto out;
	}

	tr = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct TimeRequest), ASOIOR_ReplyPort, mp, TAG_END);
	if (tr == NULL)
	{
		error = IOERR_OPENFAIL;
		goto out;
	}

	error = OpenDevice(TIMERNAME, (ULONG)unit, (struct IORequest *)tr, 0);
	if (error != 0)
		goto out;

	tr->tr_node.io_Command = command;
	tr->tr_time.tv_secs = tv->tv_secs;
	tr->tr_time.tv_micro = tv->tv_micro;

	SetSignal(0, (1UL << mp->mp_SigBit));

	error = DoIO((struct IORequest *)tr);

	tv->tv_secs = tr->tr_time.tv_secs;
	tv->tv_micro = tr->tr_time.tv_micro;

out:

	if (tr != NULL)
	{
		if (tr->tr_node.io_Device != NULL)
			CloseDevice((struct IORequest *)tr);

		FreeSysObject(ASOT_IOREQUEST, tr);
	}

	if (mp != NULL)
		FreeSysObject(ASOT_PORT, mp);

	return (error);
}
