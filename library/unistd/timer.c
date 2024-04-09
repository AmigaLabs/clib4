/*
 * $Id: unistd_timer.c,v 1.12 2024-04-09 16:35:56 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timerequest TimeRequest
#define tr_node Request

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

void _timer_init(void) {
    ENTER();

    struct _clib4 *__clib4 = __CLIB4;
    __clib4->__timer_semaphore = __create_semaphore();
    if (!__clib4->__timer_semaphore) {
        goto out;
    }

    __clib4->__timer_port = AllocSysObjectTags(ASOT_PORT,
                                               ASOPORT_Action, PA_SIGNAL,
                                               ASOPORT_Target, FindTask(NULL),
                                               ASOPORT_AllocSig, FALSE,
                                               ASOPORT_Signal, SIGB_SINGLE,
                                               TAG_DONE);

        if (__clib4->__timer_port == NULL) {
        __show_error("The timer message port could not be created.");
        goto out;
    }
    SHOWMSG("__clib4->__timer_port allocated");

    __clib4->__timer_request = AllocSysObjectTags(ASOT_IOREQUEST,
                                                  ASOMSG_Size, sizeof(struct TimeRequest),
                                                  ASOMSG_ReplyPort, __clib4->__timer_port,
                                                  TAG_DONE);
    if (__clib4->__timer_request == NULL) {
        __show_error("The timer I/O request could not be created.");
        goto out;
    }
    SHOWMSG("__clib4->__timer_request allocated");

    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)__clib4->__timer_request, 0) != OK) {
        __show_error("The timer could not be opened.");
        goto out;
    }
    SHOWMSG("OpenDevice success");

    __clib4->__TimerBase = (struct Library *) __clib4->__timer_request->tr_node.io_Device;
    SHOWPOINTER(__clib4->__TimerBase);
    __clib4->__ITimer = (struct TimerIFace *) GetInterface(__clib4->__TimerBase, "main", 1, 0);
    SHOWPOINTER(__clib4->__ITimer);
    if (__clib4->__ITimer == NULL) {
        SHOWMSG("__clib4->__ITimer is NULL");
        __show_error("The timer interface could not be obtained.");
        goto out;
    }

out:

    LEAVE();
}

void _timer_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__ITimer != NULL)
        DropInterface((struct Interface *) __clib4->__ITimer);

    __clib4->__ITimer = NULL;
    __clib4->__TimerBase = NULL;

    if (__clib4->__timer_request != NULL) {
        if (__clib4->__timer_request->tr_node.io_Device != NULL)
            CloseDevice((struct IORequest *) __clib4->__timer_request);

        FreeSysObject(ASOT_IOREQUEST, __clib4->__timer_request);
        __clib4->__timer_request = NULL;
    }

    FreeSysObject(ASOT_PORT, __clib4->__timer_port);
    __clib4->__timer_port = NULL;

    __delete_semaphore(__clib4->__timer_semaphore);
    __clib4->__timer_semaphore = NULL;

    LEAVE();
}
