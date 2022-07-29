#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <sys/ipc.h>
#include <sys/cdefs.h>
#include <features.h>

__BEGIN_DECLS

#define IPC_CREAT   001000
#define IPC_EXCL    002000
#define IPC_NOWAIT  004000
#define IPC_NOERROR 010000

/* Modeflags for extra things. */
#define SHM_CLEAR       001000    /* shm segment has never been attached */
#define IPC_DELETE      002000   /* resource is scheduled to be deleted */
#define MSG_SENDWAIT    004000 /* message queue is full (waiting on send) */
#define MSG_RECVWAIT    010000 /* message queue is empty (waiting on recv) */

#define IPC_RMID 1
#define IPC_SET 2
#define IPC_STAT 3

#define IPC_PRIVATE ((key_t)0x80000000L)

struct ipc_perm
{
    uint16_t uid;  /* Owner's UID.  Matches UID/GID types in dos/dos.h */
    uint16_t gid;  /* Owner's GID */
    uint16_t cuid; /* Creator's UID */
    uint16_t cgid; /* Creator's GID */
    uint32_t mode; /* Protection flags. */
    uint32_t seq;  /* Sequence no. */
    key_t key;     /* IPC key */
};

/* Shared memory */

#define SHM_R 0400
#define SHM_W 0200

#define SHMLBA      0x10000 /* Replace with a call to determine pagesize? */
#define SHM_RDONLY  (1L << 0)
#define SHM_RND     (1L << 0)

#define SHM_LOCK    11
#define SHM_UNLOCK  12

struct shmid_ds
{
    struct ipc_perm shm_perm;
    uint32_t shm_segsz;
    void *shm_amp;
    int shm_nattach;
    uint32_t flags;
    uint32_t shm_atime; /* attach time. */
    uint32_t shm_dtime; /* detach time. */
    uint32_t shm_ctime; /* control time. */
};

/* Message Queues */

#define MSG_R 0400
#define MSG_W 0200

typedef uint32_t msglen_t;
typedef uint32_t msgqnum_t;

struct msgsnap_head
{
    uint32_t msgsnap_size;
    uint32_t msgsnap_nmsg;
};

struct msgsnap_mhead
{
    uint32_t msgsnap_mlen;
    int32_t msgsnap_mtype;
};

struct msqid_ds
{
    struct ipc_perm msg_perm;
    void *msg_first;
    void *msg_last;
    msglen_t msg_cbytes;
    msgqnum_t msg_qnum;
    msglen_t msg_qbytes;
    int32_t msg_lspid;
    int32_t msg_lrpid;
    time_t msg_stime;
    time_t msg_rtime;
    time_t msg_ctime;
    void *Lock;
    void *WList, *RList;
};

/* Semaphores */

#define SEM_R 0400
#define SEM_W 0200

/* for semctl() */

#define GETVAL  4
#define SETVAL  5
#define GETPID  6
#define GETNCNT 7
#define GETZCNT 8
#define SETALL  9
#define GETALL  10

#define SEM_UNDO 020000

struct semid_ds
{
    struct ipc_perm sem_perm;
    void *sem_base;
    int32_t sem_nsems;
    uint32_t sem_otime;
    uint32_t sem_ctime;
    int32_t sem_binary;
    void *Lock;
};

union semun
{
    int val;
    struct semid_ds *buf;
    uint16_t *array;
};

/* IPC services control. */

extern void *_shmat(int shmid, const void *shmaddr, int shmflg);
extern int _shmctl(int shmid, int cmd, struct shmid_ds *buf);
extern int _shmdt(const void *shmaddr);
extern int _shmget(key_t key, size_t size, int shmflg);
extern int _shmids(int *buf, size_t nids, size_t *pnids);

#define shmat(a, b, c)  _shmat(a, b, c)
#define shmctl(a, b, c) _shmctl(a, b, c)
#define shmdt(a)        _shmdt(a)
#define shmget(a, b, c) _shmget(a, b, c)
#define shmids(a, b, c) _shmids(a, b, c)

__END_DECLS

#endif // _SYS_SHM_H