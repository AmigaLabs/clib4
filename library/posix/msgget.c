/*
 * $Id: msg_msgget.c,v 1.1 2021-01-26 19:32:53 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_msgget(key_t key, int flags) {
    ENTER();

    SHOWVALUE(key);
    SHOWVALUE(flags);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = msgget(key, flags);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
