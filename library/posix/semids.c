/*
 * $Id: sys_semids.c,v 1.00 2021-02-02 17:07:00 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

long int
_semids(long int *buf, uint32_t nids, long unsigned int *idcnt) {
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
    int32 ret = -1;
    if (res->haveShm) {
        ret = semids(buf, nids, idcnt);
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
