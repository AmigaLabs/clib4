/*
 * $Id: sysv_semtimedop.c,v 1.1 2023-07-08 17:26:15 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_semtimedop(int semid, const struct sembuf *ops, int nops, struct timespec *to) {
    ENTER();
    int ret = -1;

    SHOWVALUE(semid);
    SHOWPOINTER(ops);
    SHOWVALUE(nops);
    SHOWPOINTER(to);

    //struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    __set_errno(EINVAL);

    RETURN(ret);
    return ret;
}
