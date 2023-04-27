/*
 * $Id: sched_sched_yield.h,v 1.1 2021-01-14 12:06:14 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <sched.h>

int
sched_yield(void) {
    ENTER();

    // TODO - User IExec->Reschedule()
    /* SetTaskPri() on the currently running task triggers a reschedule */
    struct Task *me = FindTask(NULL);
    SetTaskPri(me, me->tc_Node.ln_Pri);

    RETURN(0);
    return 0;
}