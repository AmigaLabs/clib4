
#ifndef __IPC_PRIVATE_H__
#define __IPC_PRIVATE_H__

#include <utility/tagitem.h>
#include <proto/exec.h>
#include <dos/dosextens.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#define    DEF_SHMMAX    8388608    /* 8 MB */
#define    DEF_QSIZEMAX    4194304    /* 4 MB */
/* How many slots to add to the object list when full. */
#define VEXTEND 32

#define	DEFAULT_QSIZE	4096	/* Just a guess. Perhaps 2k would be better? 8k? 16k? */

struct Msg {
    struct Msg *Next;
    uint32 Size;
    int32 Type;
    uint8	Data[];
};

struct sem {
    int32 semval;
    uint32 sempid;
    uint16 semncnt;
    uint16 semzcnt;
    APTR zList, nList;    /* Lists of waiting processes. */
};

struct UndoInfo {
    /* Used for semaphore operation which specify the SEM_UNDO flag. Operations are undone when the interface is dropped. */
    struct UndoInfo *Next;
    int Id;         /* Semaphore id */
    int Adj[];      /* Array of adjustment values. */
};

struct IFPrivate {
    struct UndoInfo *Undo;
    int32 ErrNo;
    int32 UID, GID;
};

struct IPCGeneric {
    /* Common header for all ipc structures. */
    struct ipc_perm perm;
};

struct IPCIdKeyMap {
    int nobj, vlen, idx, nused;    /* Highest valid index + 1, vector length, total created objects, total used slots */
    struct IPCGeneric **objv;
    struct SignalSemaphore *Lock;
};

struct TimeoutCx {
    int semid;
    struct IPCIdKeyMap *km;
    struct Task *T;
    struct timespec to;
    volatile int Status;
};

/* Wait-queue support */
struct WProc {
    struct MinNode head;
    struct Task *T;
};

struct Clib4Resource;

extern struct UndoInfo *GetUndo(struct Clib4Resource *res, int id, int create);
extern void WakeList(struct MinList *ml);
extern struct WProc *RequestWakeup(struct MinList *ml);
extern void WaitList(struct WProc *wp);
extern void CancelWakeup(struct MinList *ml);
extern int GetIPCKeyId(struct IPCIdKeyMap *m, key_t key, int flags, void *(*Construct)(int, int));
extern void IPCMapUninit(struct IPCIdKeyMap *m);
extern void IPCMapInit(struct IPCIdKeyMap *m);
extern void *GetIPCById(struct IPCIdKeyMap *m, int id);
extern void IPCLock(struct IPCIdKeyMap *m);
extern void IPCUnlock(struct IPCIdKeyMap *m);
extern void *GetIPCByP(struct IPCIdKeyMap *m, int (*eq)(void *, void *), void *v, int *id);
extern void IPCRmId(struct IPCIdKeyMap *m, int id, void (*Destroy)(struct IPCGeneric *));
extern int IPCids(struct IPCIdKeyMap *m, int32 *idbuf, uint32 idblen, uint32 *idcnt);

#define    IFPRIVATE(IF)    (void *) ( ((int32)IF) - IF->Data.NegativeSize )
#define    IPCERRNO    (((struct IFPrivate *)IFPRIVATE(Self))->ErrNo)
#define    SELFPID    (((struct Process *)(FindTask(0)))->pr_ProcessID)

#endif

