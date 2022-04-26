/*
 * $Id: sys_semids.c,v 1.00 2021-02-02 17:07:00 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

long int
_semids(long int *buf, uint32_t nids, long unsigned int *idcnt)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(idcnt);

    int32 ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = semids(buf, nids, idcnt);
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