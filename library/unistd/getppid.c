/*
 * $Id: unistd_getppid.c,v 1.5 2021-02-07 19:24:11 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

pid_t
getppid(void) {
    pid_t result;

    ENTER();

    struct Task *t = FindTask(NULL);

    if (t->tc_Node.ln_Type != NT_PROCESS) {
        result = 0;
    }

    result = ((struct Process *) t)->pr_ParentID;

    RETURN(result);
    return (result);
}
