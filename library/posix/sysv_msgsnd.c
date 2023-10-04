/*
 * $Id: sysv_msgsnd.c,v 1.2 2023-07-08 18:25:10 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

int
_msgsnd(int qid, const void *msg, size_t mlen, int flags) {
    ENTER();
    struct msqid_ds *qi;
    int ret = -1;
    int bleft;
    struct Msg *m, *lm;
    struct WProc *wp;

    SHOWVALUE(qid);
    SHOWPOINTER(msg);
    SHOWVALUE(mlen);
    SHOWVALUE(flags);

    if (!msg) {
        __set_errno(EFAULT);
        RETURN(ret);
        return ret;
    }
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);

redo: /* Retry after waiting for queue to not be full. */

    wp = 0;

    IPCLock(&res->msgcx.keymap);
    qi = GetIPCById(&res->msgcx.keymap, qid);
    if (qi) {
        ObtainSemaphore(qi->Lock);
    }
    IPCUnlock(&res->msgcx.keymap);

    if (qi) {
        bleft = qi->msg_qbytes - qi->msg_cbytes;
        if (bleft >= mlen) {
            m = AllocVecTags(mlen + sizeof(struct Msg), MEMF_SHARED, TAG_DONE);
            if (m) {
                m->Next = 0;
                m->Size = mlen;
                CopyMem(msg, &m->Type, mlen + sizeof(int32));
                /* Add message */
                lm = qi->msg_last;
                if (lm) { /* Queue was not empty */
                    lm->Next = m;
                } else { /* Queue was empty */
                    qi->msg_first = m;
                }
                qi->msg_last = m;
                /* Update counters */
                qi->msg_qnum++;
                qi->msg_cbytes += mlen;

                /* Signal waiting tasks. */
                WakeList(qi->RList);
                qi->msg_perm.mode &= ~MSG_RECVWAIT;

                ret = 0;
            } else {
                __set_errno(ENOMEM);
            }
        } else if (mlen > qi->msg_qbytes) {
            if (flags & MSG_NOERROR) {
                ret = 0;
            } else {
                __set_errno(EINVAL); /* Message does not fit in queue. */
            }
        } else if (!(flags & IPC_NOWAIT)) {
            /* Suspend until space available. */
            wp = RequestWakeup(qi->WList);
            qi->msg_perm.mode |= MSG_SENDWAIT;
        } else {
            __set_errno(EAGAIN);
        }
        if (!wp) {
            qi->msg_lspid = SELFPID;
        }
        ReleaseSemaphore(qi->Lock);
    } else {
        __set_errno(EIDRM);
    }

    if (wp) {
        /* Suspend self. */
        SuspendTask(wp->T, 0);
        /* Try to follow exec autodoc and free resources in the same thread that allocated them when possible. */
        FreeSysObject(ASOT_NODE, wp);
        goto redo;
    }

    RETURN(ret);
    return ret;
}
