/*
 * $Id: sysv_msgrcv.c,v 1.2 2023-07-08 19:35:07 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

ssize_t
_msgrcv(int qid, void *msg, size_t mlen, long mtype, int flags) {
    ENTER();
    int ret = -1;

    SHOWVALUE(qid);
    SHOWPOINTER(msg);
    SHOWVALUE(mlen);
    SHOWVALUE(mtype);
    SHOWVALUE(flags);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    struct Msg *m, *prev;
    int nbytes;
    struct msqid_ds *qi;
    struct WProc *wp;

    if (!msg) {
        __set_errno(EFAULT);
        RETURN(ret);
        return ret;
    }

    redo: /* Retry after waiting for queue to be non-empty */

    wp = 0;

    IPCLock(&res->msgcx.keymap);
    qi = GetIPCById(&res->msgcx.keymap, qid);
    if (qi) {
        ObtainSemaphore(qi->Lock);
    }
    IPCUnlock(&res->msgcx.keymap);
    if (qi) {
        prev = 0;
        m = qi->msg_first;
        if (mtype > 0) {
            while (m) {
                if (m->Type == mtype) {
                    break;
                }
                prev = m;
                m = m->Next;
            }
        } else if (mtype < 0) {
            mtype = -mtype;
            while (m) {
                if (m->Type <= mtype) {
                    break;
                }
                prev = m;
                m = m->Next;
            }
        }
        if (m) {
            if (prev) { /* Remove message from queue. */
                prev->Next = m->Next;
            } else {
                qi->msg_first = m->Next;
            }
            if (qi->msg_last == m) {
                qi->msg_last = prev;
            }
            nbytes = sizeof(int32) + ((m->Size <= mlen) ? m->Size : mlen);
            if ((m->Size <= mlen) || (flags & IPC_NOERROR)) {
                CopyMem(&m->Type, msg, nbytes);
                qi->msg_cbytes -= m->Size;
                qi->msg_qnum--;
                qi->msg_lrpid = SELFPID;
                free(m);
                ret = 0;
                /* Message read, now wake all processes waiting to write. */
                WakeList(qi->WList);
                qi->msg_perm.mode &= ~MSG_SENDWAIT;
            } else {
                __set_errno(E2BIG);
            }
        } else if (!(flags & IPC_NOWAIT)) {
            /* Schedule wait for messages. */
            wp = RequestWakeup(qi->RList);
            qi->msg_perm.mode |= MSG_RECVWAIT;
        } else {
            __set_errno(ENOMSG);
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
