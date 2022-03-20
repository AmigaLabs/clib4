/*
 * $Id: shm_shmdt.c,v 1.1 2021-01-26 13:30:03 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmdt(const void *shmaddr)
{
    DECLARE_SYSVYBASE();

    int ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = shmdt(shmaddr);
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