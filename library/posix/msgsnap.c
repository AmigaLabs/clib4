/*
 * $Id: msg_msgsnap.c,v 1.1 2021-01-26 19:42:41 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_msgsnap(int msqid, void *buf, size_t bufsz, long msgtyp) {
    ENTER();

    SHOWVALUE(msqid);
    SHOWPOINTER(buf);
    SHOWVALUE(bufsz);
    SHOWVALUE(msgtyp);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = msgsnap(msqid, buf, bufsz, msgtyp);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
