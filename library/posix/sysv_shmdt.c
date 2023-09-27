/*
 * $Id: sysv_shmdt.c,v 1.2 2023-07-08 13:30:03 clib4devs Exp $
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

static int
addrcmp(struct shmid_ds *si, void *addr) {
    return (si->shm_amp == addr);
}

int
_shmdt(const void *shmaddr) {
    ENTER();
    int id;
    int ret = -1;
    struct shmid_ds *si;

    SHOWPOINTER(shmaddr);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->shmcx.keymap);
    si = GetIPCByP(&res->shmcx.keymap, (int (*)(void *, void *)) addrcmp, shmaddr, &id);
    if (si) {
        si->shm_nattach--;
        if (si->shm_nattach == 0 && (si->flags & SHMFLG_DeleteMe)) {
            res->shmcx.totshm -= si->shm_segsz;
            IPCRmId(&res->shmcx.keymap, id, (void (*)(struct IPCGeneric *)) shm_destroy);
        }
        ret = 0;
    } else {
        __set_errno(EIDRM);
    }
    /* Unlock */
    IPCUnlock(&res->shmcx.keymap);

    RETURN(ret);
    return ret;
}
