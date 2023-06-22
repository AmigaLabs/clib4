/*
 * $Id: sys_semop.c,v 1.00 2021-02-02 17:36:42 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_semop(int semid, const struct sembuf *ops, int nops) {
    DECLARE_SYSVYBASE();
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(semid);
    SHOWPOINTER(ops);
    SHOWVALUE(nops);

    int ret = -1;
    if (__clib2->haveShm) {
        ret = semop(semid, ops, nops);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    }
    else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
