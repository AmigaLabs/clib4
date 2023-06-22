/*
 * $Id: msg_msgsnd.c,v 1.1 2021-01-27 18:25:10 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_msgsnd(int msgid, const void *msg_ptr, size_t msg_sz, int msgflg) {
    DECLARE_SYSVYBASE();
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(msgid);
    SHOWPOINTER(msg_ptr);
    SHOWVALUE(msg_sz);
    SHOWVALUE(msgflg);

    int ret = -1;
    if (__clib2->haveShm) {
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
