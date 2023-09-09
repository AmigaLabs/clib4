/*
 * $Id: sysv_msgsnap.c,v 1.2 2023-07-08 19:42:41 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

int
_msgsnap(int qid, void *qbuf, size_t blen, long mtype) {
    ENTER();
    struct Msg *m;
    uint8 *bpos;
    int tlen;
    int ret = -1;
    int i, n;
    int act_cnt, act_size;
    struct msqid_ds *qi;
    const int sizemask = sizeof(uint32) - 1;
    DECLARE_UTILITYBASE();

    SHOWVALUE(qid);
    SHOWPOINTER(qbuf);
    SHOWVALUE(blen);
    SHOWVALUE(mtype);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->msgcx.keymap);
    qi = GetIPCById(&res->msgcx.keymap, qid);
    if (qi) {
        ObtainSemaphore(qi->Lock);
    }
    IPCUnlock(&res->msgcx.keymap);

    if (qi) {
        /* tlen is an estimate of the maximum length needed. The actual required length may be lower. */
        tlen = qi->msg_cbytes + sizeof(struct msgsnap_head) + qi->msg_qnum * (sizeof(struct msgsnap_mhead) + sizemask);
        if (blen < sizeof(struct msgsnap_head)) {
            __set_errno(EINVAL);
        } else if (blen < tlen) {
            if (qbuf) {
                struct msgsnap_head msg;
                msg.msgsnap_size = tlen;
                msg.msgsnap_nmsg = 0;
                CopyMem(&msg, qbuf, sizeof(struct msgsnap_head));
                ret = 0;
            } else {
                __set_errno(EFAULT);
            }
        } else if (qbuf) {
            act_cnt = 0;
            act_size = 0;
            bpos = qbuf;
            bpos += sizeof(struct msgsnap_head);
            m = qi->msg_first;
            while (m) {
                if (mtype == 0 || (mtype == m->Type) || (mtype < 0 && m->Type <= -mtype)) {
                    i = m->Size + sizeof(struct msgsnap_mhead);
                    n = (i & sizemask) ? sizeof(uint32) - (i & sizemask) : 0; /* To align to sizeof(uint32) bytes. */
                    act_cnt++;
                    act_size += (i + n);
                    CopyMem(&m->Size, bpos, i);
                    bpos += (i + n);
                }
                m = m->Next;
            }
            struct msgsnap_head msg;
            msg.msgsnap_size = act_size;
            msg.msgsnap_nmsg = act_cnt;
            CopyMem(&msg, qbuf, sizeof(struct msgsnap_head));
            ret = 0;
        } else {
            __set_errno(EFAULT);
        }
        ReleaseSemaphore(qi->Lock);
    } else {
        ret = EIDRM;
    }

    RETURN(ret);
    return ret;
}
