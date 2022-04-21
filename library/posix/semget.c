/*
 * $Id: sys_semget.c,v 1.00 2021-02-02 17:39:33 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_semget(key_t key, int nsems, int flags)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWVALUE(key);
    SHOWVALUE(nsems);
    SHOWVALUE(flags);

    int ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = semget(key, nsems, flags);
        if (ret < 0)
        {
            __set_errno(GetIPCErr());
        }
        return (ret);
    }
    else
    {
        __set_errno(ENOSYS);
    }

    return ret;
}

#endif