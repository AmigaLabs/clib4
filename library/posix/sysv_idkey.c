/*
 * $Id: ipc_ftok.c,v 1.0 2021-01-26 12:19:14 clib4devs Exp $
 *
 * Common support functions.
 *
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "ipc_headers.h"
#include "clib4.h"
#include "uuid.h"
#include "map.h"

struct UndoInfo *
GetUndo(struct Clib4Resource *res, int id, int create) {
    struct _clib4 *__clib4 = __CLIB4;
    struct UndoInfo *ui = NULL;
    struct Clib4Node key;
    memset(&key, 0, sizeof(struct Clib4Node));
    strncpy(key.uuid, __clib4->uuid, UUID4_LEN);
    struct Clib4Node *c2n = (struct Clib4Node *) hashmap_get(res->children, &key);
    if (c2n != NULL && c2n->undo != NULL) {
        ui = c2n->undo;
        while (ui) {
            if (ui->Id == id) {
                break;
            }
        }
    }
    if (create && !ui) {
        struct semid_ds *si;
        si = GetIPCById(&res->semcx.keymap, id);
        ui = AllocVecTags(sizeof(struct UndoInfo) + sizeof(int32) * si->sem_nsems,
                          AVT_Type, MEMF_SHARED,
                          AVT_ClearWithValue, 0,
                          TAG_DONE);
        ui->Id = id;
        ui->Next = c2n->undo;
        c2n->undo = ui;
        hashmap_set(res->children, c2n);
    }
    return (ui);
}

void
IPCMapInit(struct IPCIdKeyMap *m) {
    if (!m->Lock) {
        m->nobj = 0;
        m->vlen = 0;
        m->idx = 0;
        m->nused = 0;
        m->objv = 0;
        m->Lock = AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE); /* AllocSysObject() also calls InitSemaphore() */
    }
}

void
IPCMapUninit(struct IPCIdKeyMap *m) {
    if (m->Lock) {
        FreeSysObject(ASOT_SEMAPHORE, m->Lock);
        m->Lock = 0;
    }
}

/* Returns -ERROR on failure. */
int
GetIPCKeyId(struct IPCIdKeyMap *m, key_t key, int flags, void *(*Construct)(int, int)) {
    int id = -1, newidx;
    int i;
    struct IPCGeneric *o;

    newidx = m->nobj;

    if (key != IPC_PRIVATE) {
        for (i = 0; i < m->nobj; i++) {
            if (m->objv[i] != NULL && m->objv[i]->perm.key == key) {
                id = i; /* Object already exists. */
                break;
            } else if (m->objv[i] == NULL && newidx == m->nobj) {
                newidx = i; /* First free slot. */
            }
        }
    } else {
        flags |= IPC_CREAT;
    }

    if (flags & IPC_CREAT) { /* Will attempt to create resource */
        if (id >= 0 && (flags & IPC_EXCL)) { /* Exclusive resource requested, but it already exists. */
            id = -EEXIST;
        } else if (id == -1) { /* Create a new resource */
            o = (struct IPCGeneric *) Construct(key, flags);
            if (o) {
                o->perm.seq = m->idx++;
                if (m->nobj == m->vlen) { /* Add more free slots if needed. */
                    struct IPCGeneric **tmp;
                    tmp = AllocVecTags(sizeof(struct IPCGeneric *) * (m->vlen + VEXTEND),
                                            AVT_Type, MEMF_SHARED,
                                            TAG_DONE);
                    if (tmp) {
                        if (m->objv) {
                            CopyMem(m->objv, tmp, sizeof(struct shmid_ds *) * m->vlen);
                            FreeVec(m->objv);
                        }
                        m->objv = tmp;
                        m->vlen += VEXTEND;
                    }
                    else {
                        id = -ENOMEM;
                        return id;
                    }
                }
                m->objv[newidx] = o;
                if (newidx == m->nobj) {
                    m->nobj++;
                }
                id = newidx;
                m->nused++;
            }
        } /* Else already ok. */
    } else if (id == -1) {
        id = -ENOENT;
    }

    return (id);
}

void *
GetIPCById(struct IPCIdKeyMap *m, int id) {
    if (id >= 0 && id < m->nobj && m->objv[id]) {
        return (m->objv[id]);
    }
    return 0;
}

void *
GetIPCByP(struct IPCIdKeyMap *m, int (*eq)(void *, void *), void *v, int *id) {
    int i;
    for (i = 0; i < m->nobj; i++) {
        if (m->objv[i]) {
            if (eq(m->objv[i], v)) {
                if (id) {
                    *id = i;
                }
                return (m->objv[i]);
            }
        }
    }
    return (0);
}

void
IPCLock(struct IPCIdKeyMap *m) {
    ObtainSemaphore(m->Lock);
}

void
IPCUnlock(struct IPCIdKeyMap *m) {
    ReleaseSemaphore(m->Lock);
}

void
IPCRmId(struct IPCIdKeyMap *m, int id, void (*Destroy)(struct IPCGeneric *)) {
    if (GetIPCById(m, id)) {
        if (Destroy) {
            Destroy(m->objv[id]);
        }
        m->objv[id] = 0;
        m->nused--;
    }
}

int
IPCids(struct IPCIdKeyMap *m, int32 *idbuf, uint32 idblen, uint32 *idcnt) {
    int i, n;

    if (idcnt) {
        *idcnt = m->nused;
    }
    if (idblen >= m->nused) {
        for (i = n = 0; i < m->nobj; i++) {
            if (m->objv[i]) {
                idbuf[n++] = i;
            }
        }
    }

    return (0);
}

/* Support for waiting on an empty/full queue. {{{*/
void
WakeList(struct MinList *ml) {
    struct WProc *wp;
    int32 pri;

    wp = (struct WProc *) RemHead((struct List *) ml);
    if (wp) {
        while (wp) {
            while (wp->T->tc_State != TS_SUSPENDED) {
                Reschedule();
            }
            RestartTask(wp->T, 0);
            /* Do not free nodes here. They are freed by the tasks which allocated then. */
            wp = (struct WProc *) RemHead((struct List *) ml);
        }
    }
}

struct WProc *
RequestWakeup(struct MinList *ml) {
    struct WProc *wp;
    wp = AllocSysObjectTags(ASOT_NODE,
                            ASONODE_Size, sizeof(struct WProc),
                            ASONODE_Min, TRUE,
                            TAG_DONE);
    wp->T = FindTask(0);
    AddTail((struct List *) ml, (struct Node *) wp);
    return (wp);
}

void
CancelWakeup(struct MinList *ml) {
    struct WProc *wp;
    wp = (struct WProc *) RemTail((struct List *) ml);
    if (wp) {
        FreeSysObject(ASOT_NODE, wp);
    }
}

void
WaitList(struct WProc *wp) {
    /* Suspend self. */
    SuspendTask(wp->T, 0);
    /* Try to follow exec autodoc and free resources in the same thread that allocated them when possible. */
    FreeSysObject(ASOT_NODE, wp);
}

