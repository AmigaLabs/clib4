/*
 * $Id: unistd_pause.c,v 1.0 2022-03-15 08:39:45 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int pause(void) {
	ENTER();
    struct _clib4 *__clib4 = __CLIB4;

	uint32 signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E | ( 1L << __clib4->__timer_port->mp_SigBit ) );
	if (signals & SIGBREAKF_CTRL_C) {
		SHOWMSG("Received SIGINT");
		raise(SIGINT);
	}
	
	__set_errno_r(__clib4, EINTR);
	
	RETURN(-1);

    return -1;
}