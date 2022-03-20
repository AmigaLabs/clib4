/*
 * $Id: shm_shmget.c,v 1.1 2021-01-26 13:24:59 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmget(key_t key, size_t size, int flags)
{
    DECLARE_SYSVYBASE();

    int ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = shmget(key, size, flags);
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