/*
 * $Id: sysv_shmids.c,v 1.2 2023-07-08 13:40:49 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_shmids(int *buf, size_t nids, size_t *idcnt) {
    ENTER();
    int ret = -1;

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(idcnt);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->shmcx.keymap);
    ret = IPCids(&res->shmcx.keymap, (int32 *) buf, nids, (uint32 *) idcnt);
    if (ret) {
        __set_errno(ret);
        ret = -1;
    }
    IPCUnlock(&res->shmcx.keymap);

    RETURN(ret);
    return ret;
}
