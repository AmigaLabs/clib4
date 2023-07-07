/*
 * $Id: msg_msgsnd.c,v 1.1 2021-01-27 18:25:10 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_msgsnd(int msgid, const void *msg_ptr, size_t msg_sz, int msgflg) {
    ENTER();

    SHOWVALUE(msgid);
    SHOWPOINTER(msg_ptr);
    SHOWVALUE(msg_sz);
    SHOWVALUE(msgflg);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (!res) {
        __set_errno(ENOSYS);
        return -1;
    }

    DECLARE_SYSVYBASE();
    int ret = -1;
    if (res->haveShm) {
        ret = msgsnd(msgid, msg_ptr, msg_sz, msgflg);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    RETURN(ret);
    return ret;
}
