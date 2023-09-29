/*
 * $Id: sysv_msgctl.c,v 1.2 2023-07-08 13:59:43 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

static void msg_destroy(struct msqid_ds *qi) {
    struct Msg *m, *rm;
    if(qi) {
        m=qi->msg_first;
        while(m) {
            rm=m;
            m=m->Next;
            FreeVec(rm);
        }
        ReleaseSemaphore(qi->Lock);
        FreeSysObject(ASOT_SEMAPHORE,qi->Lock);
        FreeSysObject(ASOT_LIST,qi->WList);
        FreeSysObject(ASOT_LIST,qi->RList);
        FreeVec(qi);
    }
}

int
_msgctl(int qid, int cmd, struct msqid_ds *buf) {
    ENTER();

    SHOWVALUE(qid);
    SHOWVALUE(cmd);
    SHOWPOINTER(buf);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    int ret = -1;
    struct msqid_ds *qi;
    IPCLock(&res->msgcx.keymap);
    qi = GetIPCById(&res->msgcx.keymap, qid);
    if (qi) {
        ObtainSemaphore(qi->Lock);
        switch (cmd) {
            case IPC_STAT:
                if (buf)
                    CopyMem(qi, buf, sizeof(struct msqid_ds));
                else
                    __set_errno(EFAULT);
                break;
            case IPC_SET:
                if (buf->msg_qbytes > 0) {
                    /* TODO: Check permissions. */
                    qi->msg_qbytes = buf->msg_qbytes;
                } else
                    __set_errno(EINVAL);
                break;
            case IPC_RMID:
                /* Schedule all tasks waiting on this queue so they can fail. */
                WakeList(qi->WList);
                WakeList(qi->RList);
                /* And remove the queue. */
                IPCRmId(&res->msgcx.keymap, qid, (void (*)(struct IPCGeneric *)) msg_destroy);
                qi = 0;
                break;
            default:
                __set_errno(EINVAL);
        }
        ReleaseSemaphore(qi->Lock);
    } else {
        __set_errno(EIDRM);
    }
    IPCUnlock(&res->msgcx.keymap);

    RETURN(ret);
    return ret;
}
