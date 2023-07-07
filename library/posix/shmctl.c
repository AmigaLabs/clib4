/*
 * $Id: shm_shmctl.c,v 1.1 2021-01-26 13:36:17 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmctl(int shmid, int cmd, struct shmid_ds *cbuf) {
    ENTER();

    SHOWVALUE(shmid);
    SHOWVALUE(cmd);
    SHOWPOINTER(cbuf);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = shmctl(shmid, cmd, cbuf);
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
