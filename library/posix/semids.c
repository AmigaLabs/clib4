/*
 * $Id: sys_semids.c,v 1.00 2021-02-02 17:07:00 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_semids(int32_t *buf, uint32_t nids, uint32_t *idcnt)
{
    DECLARE_SYSVYBASE();
        
    int ret = -1;
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

    return ret;
}

#endif