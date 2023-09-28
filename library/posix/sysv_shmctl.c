/*
 * $Id: sysv_shmctl.c,v 1.2 2023-07-08 13:36:17 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

static void
shm_destroy(struct shmid_ds *si) {
    if (si) {
        if (si->shm_amp) {
            FreeVec(si->shm_amp);
        }
        FreeVec(si);
    }
}

int
_shmctl(int shmid, int cmd, struct shmid_ds *cbuf) {
    ENTER();
    int ret = -1;
    struct shmid_ds *si;

    SHOWVALUE(shmid);
    SHOWVALUE(cmd);
    SHOWPOINTER(cbuf);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->shmcx.keymap);
    si = GetIPCById(&res->shmcx.keymap, shmid);
    if (si) {
        switch (cmd) {
            case IPC_STAT:
                if (cbuf) {
                    CopyMem(si, cbuf, sizeof(struct shmid_ds));
                    ret = 0;
                } else {
                    __set_errno(EFAULT);
                }
                break;
            case IPC_SET: /* Silently ignore. */
                ret = 0;
                break;
            case IPC_RMID:
                si->flags |= SHMFLG_DeleteMe;
                si->shm_perm.mode |= IPC_DELETE;
                if (si->shm_nattach == 0) {
                    IPCRmId(&res->shmcx.keymap, shmid, (void (*)(struct IPCGeneric *)) shm_destroy);
                }
                ret = 0;
                break;
            case SHM_LOCK:
                if (LockMem(si->shm_amp, si->shm_segsz) == FALSE) {
                    __set_errno(EACCES);
                } else {
                    ret = 0;
                }
                break;
            case SHM_UNLOCK:
                LockMem(si->shm_amp, si->shm_segsz);
                ret = 0;
                break;
            default:
                __set_errno(EINVAL);
        }
    } else {
        __set_errno(EIDRM);
    }
    /* Unlock */
    IPCUnlock(&res->shmcx.keymap);

    RETURN(ret);
    return ret;
}
