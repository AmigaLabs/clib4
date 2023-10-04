/*
 * $Id: sysv_semctl.c,v 1.1 2023-07-08 17:07:00 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

static void
sem_destroy(struct semid_ds *si) {
    struct sem *sa;
    int i;
    if (si) {
        /* Wake everyone who is waiting on this semaphore array so they can fail. */
        sa = si->sem_base;
        for (i = 0; i < si->sem_nsems; i++) {
            WakeList(sa[i].zList);
            WakeList(sa[i].nList);
            FreeSysObject(ASOT_LIST, sa[i].zList);
            FreeSysObject(ASOT_LIST, sa[i].nList);
        }
        FreeSysObject(ASOT_SEMAPHORE, si->Lock);
        FreeVec(si->sem_base);
        FreeVec(si);
    }
}

int
_semctl(int semid, int semnum, int cmd, union semun aun) {
    ENTER();
    int ret = -1;
    struct semid_ds *si;
    struct sem *sb;
    int i;
    struct UndoInfo *ui;

    SHOWVALUE(semid);
    SHOWVALUE(semnum);
    SHOWVALUE(cmd);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->semcx.keymap);

    si = GetIPCById(&res->semcx.keymap, semid);
    if (si) {
        sb = (struct sem *) si->sem_base;
        ui = GetUndo(res, semid, 0);
        switch (cmd) {
            case IPC_STAT:
                if (aun.buf) {
                    CopyMem(si, aun.buf, sizeof(struct semid_ds));
                    ret = 0;
                } else {
                    __set_errno(EFAULT);
                }
                break;
            case IPC_SET: /* Silently ignore. */
                ret = 0;
                break;
            case IPC_RMID:
                IPCRmId(&res->semcx.keymap, semid, (void (*)(struct IPCGeneric *))sem_destroy);
                ret = 0;
                break;
            case GETVAL:
                if (semnum >= 0 && semnum < si->sem_nsems) {
                    ret = sb[semnum].semval;
                } else {
                    __set_errno(EINVAL);
                }
                break;
            case SETVAL:
                if (semnum >= 0 && semnum < si->sem_nsems) {
                    sb[semnum].semval = aun.val;
                    if (ui) { /* Set adj value to 0 */
                        ui->Adj[semnum] = 0;
                    }
                    ret = 0;
                } else {
                    __set_errno(EINVAL);
                }
                break;
            case GETPID:
                ret = sb[semnum].sempid;
                break;
            case GETNCNT:
                if (semnum >= 0 && semnum < si->sem_nsems) {
                    ret = sb[semnum].semncnt;
                } else {
                    __set_errno(EINVAL);
                }
                break;
            case GETZCNT:
                if (semnum >= 0 && semnum < si->sem_nsems) {
                    ret = sb[semnum].semzcnt;
                } else {
                    __set_errno(EINVAL);
                }
                break;
            case SETALL:
                if (aun.array) {
                    for (i = 0; i < si->sem_nsems; i++) {
                        if (ui) { /* Set adj value to 0 */
                            ui->Adj[semnum] = 0;
                        }
                        sb[i].semval = aun.array[i];
                    }
                    ret = 0;
                } else {
                    __set_errno(EFAULT);
                }
                break;
            case GETALL:
                if (aun.array) {
                    for (i = 0; i < si->sem_nsems; i++) {
                        aun.array[i] = sb[i].semval;
                    }
                    ret = 0;
                } else {
                    __set_errno(EFAULT);
                }
                break;
            default:
                __set_errno(EINVAL);
        }
    } else {
        ret = EIDRM;
    }
    /* Unlock */
    IPCUnlock(&res->semcx.keymap);

    RETURN(ret);
    return ret;
}
