/*
 * $Id: shm_shmdt.c,v 1.1 2021-01-26 13:30:03 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmdt(const void *shmaddr) {
    DECLARE_SYSVYBASE();
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWPOINTER(shmaddr);

    int ret = -1;
    if (__clib2->haveShm) {
        ret = shmdt(shmaddr);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    }
    else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
