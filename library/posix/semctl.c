/*
 * $Id: sys_semctl.c,v 1.00 2021-02-02 17:07:00 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_semctl(int semid, int semnum, int cmd, union semun aun)
{
    DECLARE_SYSVYBASE();

    ENTER();

    SHOWVALUE(semid);
    SHOWVALUE(semnum);
    SHOWVALUE(cmd);

    int ret = -1;
    if (__getclib2()->haveShm)
    {
        ret = semctl(semid, semnum, cmd, aun);
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