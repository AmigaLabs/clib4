/*
 * $Id: sysv_shmat.c,v 1.2 2023-07-08 12:28:14 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

void *
_shmat(int shmid, const void *prefadds, int flags) {
    ENTER();
    int i;
    void *addr = (void *) -1L;
    struct shmid_ds *si;

    SHOWVALUE(shmid);
    SHOWPOINTER(prefadds);
    SHOWVALUE(flags);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->shmcx.keymap);

    si = GetIPCById(&res->shmcx.keymap, shmid);
    if (si) {
        /* Disallow attaching if marked for deletion? */
        if (prefadds == NULL || prefadds == si->shm_amp) {
            si->shm_nattach++;
            addr = si->shm_amp;
            if (si->shm_perm.mode & SHM_CLEAR) {
                /* Clear memory first time it is attached. Could use utility.library/ClearMem instead. */
                for (i = 0; i < (si->shm_segsz >> 2); i++) {
                    ((uint32 *) addr)[i] = 0;
                }
                for (i <<= 2; i < si->shm_segsz; i++) {
                    ((uint8 *) addr)[i] = 0;
                }
                si->shm_perm.mode &= ~SHM_CLEAR;
            }
        } else {
            __set_errno(EINVAL);
        }
    } else {
        __set_errno(EIDRM);
    }

    /* Unlock */
    IPCUnlock(&res->shmcx.keymap);

    RETURN(addr);
    return (addr);
}
