/*
 * $Id: sysv_semget.c,v 1.1 2023-07-08 17:39:33 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

static void
sem_setup(struct sem *s) {
    s->semval = 1;
    s->sempid = 0;
    s->semncnt = 0;
    s->semzcnt = 0;
    s->zList = AllocSysObjectTags(ASOT_LIST, ASOLIST_Min, TRUE, TAG_DONE);
    s->nList = AllocSysObjectTags(ASOT_LIST, ASOLIST_Min, TRUE, TAG_DONE);
}

static void sem_destroy(struct semid_ds *si) {
    if (si) {
        struct sem *sa;
        int i;
        /* Wake everyone who is waiting on this semaphore array so they can fail. */
        sa = si->sem_base;
        for (i = 0; i < si->sem_nsems; i++) {
            WakeList(sa[i].zList);
            WakeList(sa[i].nList);
            FreeSysObject(ASOT_LIST, sa[i].zList);
            FreeSysObject(ASOT_LIST, sa[i].nList);
        }
        FreeSysObject(ASOT_SEMAPHORE, si->Lock);
        free(si->sem_base);
        free(si);
    }
}

static struct semid_ds *sem_construct(int key, int flags) {
    struct semid_ds *si;
    si = calloc(1, sizeof(struct semid_ds));
    if (si) {
        si->sem_base = 0;
        si->sem_perm.mode = flags & 0777;
        si->sem_perm.key = key;
        si->sem_nsems = 0;
        si->sem_binary = 0;
        si->Lock = AllocSysObject(ASOT_SEMAPHORE, TAG_DONE);
    }
    return (si);
}

int
_semget(key_t key, int nsems, int flags) {
    ENTER();
    int id = -1;
    int i;
    struct sem *sa;
    struct semid_ds *si;

    SHOWVALUE(key);
    SHOWVALUE(nsems);
    SHOWVALUE(flags);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);

    IPCLock(&res->semcx.keymap);
    if (1) {
        /* TODO: Implement system-wide limit on number of semaphores? */
        id = GetIPCKeyId(&res->semcx.keymap, key, flags, (void *(*)(int, int)) sem_construct);
        if (id >= 0) {
            si = GetIPCById(&res->semcx.keymap, id);
            if (si && !si->sem_base) {
                si->sem_base = malloc(sizeof(struct sem) * nsems);
                if (si->sem_base) {
                    sa = si->sem_base;
                    for (i = 0; i < nsems; i++) {
                        sem_setup(&sa[i]);
                    }
                    si->sem_nsems = nsems;
                } else {
                    IPCRmId(&res->semcx.keymap, id, (void (*)(struct IPCGeneric *)) sem_destroy);
                    id = ENOMEM;
                }
            }
        }
    } else {
        id = ENOMEM;
    }

    IPCUnlock(&res->semcx.keymap);

    RETURN(id);
    return id;
}
