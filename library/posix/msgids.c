/*
 * $Id: msg_msgids.c,v 1.1 2021-01-26 19:56:21 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_msgids(int *buf, size_t nids, size_t *pnids)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(pnids);

    int ret = -1;
    if (__CLIB2->haveShm) {
        ret = msgids((int32 *)buf, nids, (uint32 *)pnids);
        if (ret < 0)
        {
            __set_errno(GetIPCErr());
        }
    }
    else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
