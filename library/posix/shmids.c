/*
 * $Id: shm_shmids.c,v 1.1 2021-01-26 13:40:49 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmids(int *buf, size_t nids, size_t *idcnt)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(idcnt);

    int ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = shmids((APTR)buf, nids, (uint32 *)idcnt);
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