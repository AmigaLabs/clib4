/*
 * $Id: msg_msgsnap.c,v 1.1 2021-01-26 19:42:41 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_msgsnap(int msqid, void *buf, size_t bufsz, long msgtyp)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWVALUE(msqid);
    SHOWPOINTER(buf);
    SHOWVALUE(bufsz);
    SHOWVALUE(msgtyp);

    int ret = -1;
    if (__CLIB2->haveShm) {
        ret = msgsnap(msqid, buf, bufsz, msgtyp);
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
