/*
 * $Id: sysv_semop.c,v 1.1 2023-07-08 17:36:42 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_semop(int semid, const struct sembuf *ops, int nops) {
    ENTER();
    struct semid_ds *si;
    int ret = -1;
    struct WProc *wp;
    int i, n = 0, t;
    int zW = 0, nW = 0;
    struct sem *sa;
    struct UndoInfo *ui = 0;
    uint32 mypid;

    SHOWVALUE(semid);
    SHOWPOINTER(ops);
    SHOWVALUE(nops);

    if (!ops) {
        __set_errno(EFAULT);
        goto out;
    }
    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);

redo:

    wp = 0;

    /* Lock */
    IPCLock(&res->semcx.keymap);
    si = GetIPCById(&res->semcx.keymap, semid);
    if (si) {
        ObtainSemaphore(si->Lock);
    }
    IPCUnlock(&res->semcx.keymap);

    if (si) {
        sa = si->sem_base;
        if (zW) {
            sa[n].semzcnt--;
        }
        if (nW) {
            sa[n].semncnt--;
        }
        zW = nW = 0;
        for (i = 0; i < nops; i++) {
            n = ops[i].sem_num;
            if (n < 0 || n > si->sem_nsems) {
                __set_errno(EFBIG);
                break;
            }
            if ((ops[i].sem_flg & SEM_UNDO) && !ui) {
                ui = GetUndo(res, semid, 1);
            }
            if (ops[i].sem_op > 0) { ; /* Nothing to do here, incrementing a semaphore always succeeds. */
            } else if (ops[i].sem_op < 0) {
                if (sa[n].semval < -ops[i].sem_op) {
                    if (ops[i].sem_flg & IPC_NOWAIT) {
                        __set_errno(EAGAIN);
                    } else {
                        sa[n].semncnt++;
                        nW = 1;
                        wp = RequestWakeup(sa[n].nList);
                    }
                    break;
                }
            } else { /* ==0 */
                if (sa[n].semval > 0) {
                    if (ops[i].sem_flg & IPC_NOWAIT) {
                        __set_errno(EAGAIN);
                    } else {
                        sa[n].semzcnt++;
                        zW = 1;
                        wp = RequestWakeup(sa[n].zList);
                    }
                    break;
                }
            }
        }
        if (i == nops) { /* All operations ok. Now we do the actual modification of the values. */
            mypid = SELFPID;
            for (i = 0; i < nops; i++) {
                n = ops[i].sem_num;
                t = ops[i].sem_op;
                sa[n].semval += t;
                sa[n].sempid = mypid;
                if (ops[i].sem_flg & SEM_UNDO) {
                    ui->Adj[n] -= t;
                }
                if (sa[n].semval == 0) {
                    WakeList(sa[n].zList);
                } else if (t > 0) {
                    WakeList(sa[n].nList);
                }
            }
            ret = 0;
        }
        ReleaseSemaphore(si->Lock);
    } else {
        __set_errno(EIDRM);
    }

    if (wp) {
        WaitList(wp);
        goto redo;
    }

out:
    
    RETURN(ret);
    return ret;
}
