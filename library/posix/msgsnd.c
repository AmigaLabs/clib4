

/*
 * $Id: msg_msgsnd.c,v 1.1 2021-01-27 18:25:10 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_msgsnd(int msgid, const void *msg_ptr, size_t msg_sz, int msgflg)
{
    DECLARE_SYSVYBASE();

    int ret = -1;
    if (__global_clib2->haveShm)
    {
        ret = msgsnd(msgid, msg_ptr, msg_sz, msgflg);
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