/*
 * $Id: sysv_msgids.c,v 1.2 2023-07-08 19:56:21 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

int
_msgids(int *buf, size_t nids, size_t *qcnt) {
    ENTER();
    int ret = -1;

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(qcnt);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->msgcx.keymap);
    ret = IPCids(&res->msgcx.keymap, (int32 *) buf, nids, (uint32 *) qcnt);
    if (ret) {
        __set_errno(ret);
        ret = -1;
    }
    IPCUnlock(&res->msgcx.keymap);

    RETURN(ret);
    return ret;
}
