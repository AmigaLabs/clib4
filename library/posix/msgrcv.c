/*
 * $Id: msg_msgrcv.c,v 1.1 2021-01-26 19:35:07 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

ssize_t
_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWVALUE(msqid);
    SHOWPOINTER(msgp);
    SHOWVALUE(msgsz);
    SHOWVALUE(msgtyp);
    SHOWVALUE(msgflg);

    int ret = -1;
    if (__getclib2()->haveShm)
    {
        ret = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
        if (ret < 0)
        {
            __set_errno(GetIPCErr());
        }
    }
    else
    {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}

#endif