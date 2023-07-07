/*
 * $Id: sys_semget.c,v 1.00 2021-02-02 17:39:33 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_semget(key_t key, int nsems, int flags) {
    ENTER();

    SHOWVALUE(key);
    SHOWVALUE(nsems);
    SHOWVALUE(flags);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = semget(key, nsems, flags);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
        return (ret);
    }
    else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
