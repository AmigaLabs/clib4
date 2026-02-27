/*
 * $Id: time_settimeofday.c,v 1.0 2022-02-24 18:31:57 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
settimeofday(const struct timeval *t, const struct timezone *tz) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;
    struct MsgPort *tmr_real_mp = NULL;
    struct TimeRequest *tmr_real_tr = NULL;
    int result = -1;
    int32 __gmtoffset = 0;

    DECLARE_TIMERBASE_R(__clib4);
    DECLARE_TIMEZONEBASE_R(__clib4);

  	SHOWMSG("AllocSysObjectTags ASOT_PORT");
    /* Create itimer timers and message ports */
    tmr_real_mp = AllocSysObjectTags(ASOT_PORT,
                                     ASOPORT_Action, PA_SIGNAL,
                                     ASOPORT_AllocSig, FALSE,
                                     ASOPORT_Signal, SIGB_SINGLE,
                                     ASOPORT_Target, FindTask(NULL),
                                     TAG_DONE);
    if (!tmr_real_mp) {
		/* Simulating that timer is busy */
       	__set_errno_r(__clib4, EPERM);
        RETURN(-1);
        return -1;
    }

    SHOWMSG("AllocSysObjectTags ASOT_IOREQUEST");
    tmr_real_tr = AllocSysObjectTags(ASOT_IOREQUEST,
                                     ASOIOR_ReplyPort, tmr_real_mp,
                                     ASOIOR_Size, sizeof(struct TimeRequest),
                                     TAG_END);
    if (!tmr_real_tr) {
		SHOWMSG("FreeSysObject ASOT_PORT");
        FreeSysObject(ASOT_PORT, tmr_real_mp);

		/* Simulating that timer is busy */
       	__set_errno_r(__clib4, EPERM);
        RETURN(-1);
        return -1;
    }
    
	SHOWMSG("OpenDevice");
    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *) tmr_real_tr, 0) != OK) {
		SHOWMSG("FreeSysObject ASOT_IOREQUEST");
        FreeSysObject(ASOT_IOREQUEST, tmr_real_tr);

		SHOWMSG("FreeSysObject ASOT_PORT");
        FreeSysObject(ASOT_PORT, tmr_real_mp);

		/* Simulating that timer is busy */
       	__set_errno_r(__clib4, EPERM);
        RETURN(-1);
        return -1;
    }	

    tmr_real_tr->Request.io_Command = TR_SETSYSTIME;
    if (tz != NULL) {
        __gmtoffset = tz->tz_minuteswest;
    }
    /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
    tmr_real_tr->Time.Seconds = t->tv_sec - ((2922 * 24 * 60 + __gmtoffset) * 60);
    tmr_real_tr->Time.Microseconds = t->tv_usec;

    DoIO((struct IORequest *)tmr_real_tr);
    GetMsg(tmr_real_mp);
    
	SHOWMSG("CloseDevice");
    CloseDevice((struct IORequest *) tmr_real_tr);
 	
	SHOWMSG("FreeSysObject ASOT_IOREQUEST");    
	FreeSysObject(ASOT_IOREQUEST, tmr_real_tr);

	SHOWMSG("FreeSysObject ASOT_PORT");
    FreeSysObject(ASOT_PORT, tmr_real_mp);
	
    result = 0;
    __set_errno_r(__clib4, 0);

    RETURN(result);
    return result;
}