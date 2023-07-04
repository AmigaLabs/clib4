/*
 * $Id: sched_sched_yield.h,v 1.1 2021-01-14 12:06:14 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <sched.h>

int
sched_yield(void) {
    Reschedule();

    return 0;
}