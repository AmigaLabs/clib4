/*
 * $Id: sys_semctl.c,v 1.00 2021-02-02 17:07:00 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_semctl(int semid, int semnum, int cmd, union semun aun) {
    ENTER();

    SHOWVALUE(semid);
    SHOWVALUE(semnum);
    SHOWVALUE(cmd);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = semctl(semid, semnum, cmd, aun);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
