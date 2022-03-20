/*
 * $Id: unistd_headers.h,v 1.12 2006-09-27 09:40:06 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#define _UNISTD_HEADERS_H

/****************************************************************************/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

extern char NOCOMMON __current_path_name[MAXPATHLEN];

/****************************************************************************/

/* Names of files and directories to delete when shutting down. */
extern struct MinList NOCOMMON			__unlink_list;
extern struct SignalSemaphore NOCOMMON	__unlink_semaphore;

/****************************************************************************/

/* Local timer I/O. */
extern struct MsgPort *	NOCOMMON __timer_port;
extern BOOL				NOCOMMON __timer_busy;
extern struct Library *	NOCOMMON __TimerBase;
extern struct TimerIFace * NOCOMMON __ITimer;

/****************************************************************************/

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

extern struct TimeRequest *	NOCOMMON __timer_request;

#else

extern struct timerequest *	NOCOMMON __timer_request;

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */


/* If the program's current directory was changed, here is where
   we find out about it. */
extern BPTR NOCOMMON __original_current_directory;
extern BOOL NOCOMMON __current_directory_changed;
extern BOOL NOCOMMON __unlock_current_directory;

/****************************************************************************/

extern int __set_current_path(const char * path_name);
extern int __strip_double_slash(char * file_name,int len);

/****************************************************************************/

extern int __time_delay(ULONG timercmd, struct timeval *tv);

/****************************************************************************/

extern long __pathconf(struct MsgPort *port,int name);

/****************************************************************************/

extern int itimer_real_task(void);

#endif /* _UNISTD_HEADERS_H */
