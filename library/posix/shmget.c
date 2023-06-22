/*
 * $Id: shm_shmget.c,v 1.1 2021-01-26 13:24:59 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int 
_shmget(key_t key, size_t size, int flags) {
    DECLARE_SYSVYBASE();
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(key);
    SHOWVALUE(size);
    SHOWVALUE(flags);

    int ret = -1;
    if (__clib2->haveShm) {
        ret = shmget(key, size, flags);
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
