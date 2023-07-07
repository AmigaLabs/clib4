/*
 * $Id: msg_msgrcv.c,v 1.1 2021-01-26 19:35:07 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

ssize_t
_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
    ENTER();

    SHOWVALUE(msqid);
    SHOWPOINTER(msgp);
    SHOWVALUE(msgsz);
    SHOWVALUE(msgtyp);
    SHOWVALUE(msgflg);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
