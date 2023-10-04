/*
 * $Id: unistd_headers.h,v 2.0 2023-05-03 09:40:06 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#define _UNISTD_HEADERS_H

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

extern int __set_current_path(const char * path_name);
extern int __strip_double_slash(char * file_name,int len);
extern int __time_delay(ULONG timercmd, struct timeval *tv);
extern long __pathconf(struct MsgPort *port,int name);
extern int itimer_real_task(void);
extern void killitimer(void);

#endif /* _UNISTD_HEADERS_H */
