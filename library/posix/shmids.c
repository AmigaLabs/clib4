/*
 * $Id: shm_shmids.c,v 1.1 2021-01-26 13:40:49 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmids(int *buf, size_t nids, size_t *idcnt) {
    ENTER();

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(idcnt);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = shmids((APTR)buf, nids, (uint32 *)idcnt);
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
