/*
 * $Id: sysv_shmget.c,v 1.2 2023-07-08 13:24:59 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

static struct shmid_ds *
shm_construct(int key, int flags) {
    struct shmid_ds *si;
    si = calloc(1, sizeof(struct shmid_ds));
    if (si) {
        si->shm_amp = 0;
        si->shm_perm.mode = (flags & 0777) | SHM_CLEAR;
        si->shm_perm.key = key;
        si->shm_segsz = 0;
    }
    return si;
}

static void
shm_destroy(struct shmid_ds *si) {
    if (si) {
        if (si->shm_amp) {
            free(si->shm_amp);
        }
        free(si);
    }
}

int
_shmget(key_t key, size_t size, int flags) {
    ENTER();
    int id = -1;
    struct shmid_ds *si;

    SHOWVALUE(key);
    SHOWVALUE(size);
    SHOWVALUE(flags);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->shmcx.keymap);

    if ((size + res->shmcx.totshm) < res->shmcx.shmmax) {
        id = GetIPCKeyId(&res->shmcx.keymap, key, flags, (void *(*)(int, int)) shm_construct);
        if (id >= 0) {
            si = GetIPCById(&res->shmcx.keymap, id);
            if (si && !si->shm_amp) {
                si->shm_amp = malloc(size);
                if (si->shm_amp) {
                    res->shmcx.totshm += size;
                    si->shm_segsz = size;
                } else {
                    IPCRmId(&res->shmcx.keymap, id, (void (*)(struct IPCGeneric *)) shm_destroy);
                    __set_errno(ENOMEM);
                    id = -1;
                }
            }
        } else {
            __set_errno(-id);
            id = -1;
        }
    } else {
        __set_errno(ENOMEM);
    }

    IPCUnlock(&res->shmcx.keymap);

    RETURN(id);
    return id;
}
