/*
 * $Id: unistd_time_delay.c,v 1.9 2006-04-05 08:39:45 obarthel Exp $
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

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif /* PROTO_TIMER_H */

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

/* amiga_dotimer.c is a different implementation and use a different timer.device
 * we can't use the same implementation because it pass a Unit parameter.
 * However SIGBREAKF_CTRL_C handling should be implemented in the same way
 */

int 
__time_delay(ULONG timercmd, struct timeval *tv)
{
	ENTER();

	struct MsgPort *mp;
	struct TimeRequest *tr;
	ULONG wait_mask;
	int result = 0;

	if (__check_abort_enabled)
		__check_abort();

	if (__timer_request == NULL)
		return EINVAL;

	SetSignal(0, SIGB_SINGLE | SIGBREAKF_CTRL_C);

	mp = AllocSysObjectTags(ASOT_PORT,
							ASOPORT_AllocSig, FALSE,
							ASOPORT_Signal, SIGB_SINGLE,
							TAG_DONE);

	tr = AllocSysObjectTags(ASOT_IOREQUEST,
							ASOIOR_Duplicate, __timer_request,
							ASOIOR_ReplyPort, mp,
							ASOIOR_Size, sizeof(struct TimeRequest),
							TAG_END);
	if (!tr)
	{
		FreeSysObject(ASOT_IOREQUEST, mp);
		return ENOMEM;
	}

	tr->Request.io_Command = timercmd;
	tr->Time.Seconds = tv->tv_sec;
	tr->Time.Microseconds = tv->tv_usec;

	SendIO((struct IORequest *)tr);
	wait_mask = SIGBREAKF_CTRL_C | 1L << mp->mp_SigBit;
	if (Wait(wait_mask) & (SIGBREAKF_CTRL_C)) {
		if (CheckIO((struct IORequest *)tr))  /* If request is complete... */
			WaitIO((struct IORequest *)tr);   /* clean up and remove reply */
		AbortIO((struct IORequest *)tr);
		/* Return EINTR since the request has been interrupted */
		result = EINTR;
	}
	WaitIO((struct IORequest *)tr);

	tv->tv_sec = tr->Time.Seconds;
	tv->tv_usec = tr->Time.Microseconds;

	FreeSysObject(ASOT_MESSAGE, tr);
	FreeSysObject(ASOT_PORT, mp);

	RETURN(result);
	return result;
}
