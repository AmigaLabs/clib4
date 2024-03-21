#ifndef _COMMON_H
#define _COMMON_H

#include <proto/dos.h>

#include "pthread.h"
#include <sys/time.h>

struct pthread_barrier {
    unsigned int curr_height;
    unsigned int total_height;
    pthread_cond_t breeched;
    pthread_mutex_t lock;
};

struct sema {
    struct Node node;
    int value;
    int waiters_count;
    pthread_mutex_t lock;
    pthread_cond_t count_nonzero;
};

#undef NEWLIST
#define NEWLIST(_l)                                     \
do                                                      \
{                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred = (struct Node *)l;                \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (struct Node *)&l->lh_Tail;      \
} while (0)

#define TIMESPEC_TO_OLD_TIMEVAL(tv, ts) {    \
    (tv)->Seconds = (ts)->tv_sec;        \
    (tv)->Microseconds = (ts)->tv_nsec / 1000; }
#define OLD_TIMEVAL_TO_TIMESPEC(tv, ts) {    \
    (ts)->tv_sec = (tv)->Seconds;        \
    (ts)->tv_nsec = (tv)->Microseconds * 1000; }

enum threadState
{
    THREAD_STATE_IDLE 		= 0,
    THREAD_STATE_RUNNING 	= 1,
    THREAD_STATE_JOINING	= 2,
    THREAD_STATE_TERMINATED	= 3,
    THREAD_STATE_CANCELED	= 4,
    THREAD_STATE_WAITING	= 5,
    THREAD_STATE_DESTRUCT   = 6,
};

#define GetNodeName(node) ((struct Node *)node)->ln_Name

#define ForeachNode(l,n) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
    ((struct Node *)(n))->ln_Succ; \
    n=(void *)(((struct Node *)(n))->ln_Succ))

#define ForeachNodeSafe(l,n,n2) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
        (n2=(void *)((struct Node *)(n))->ln_Succ); \
        n=(void *)n2)

#define SIGB_PARENT SIGBREAKB_CTRL_F
#define SIGF_PARENT (1 << SIGB_PARENT)
#define SIGB_COND_FALLBACK SIGBREAKB_CTRL_E
#define SIGF_COND_FALLBACK (1 << SIGB_COND_FALLBACK)
#define SIGB_TIMER_FALLBACK SIGBREAKB_CTRL_D
#define SIGF_TIMER_FALLBACK (1 << SIGB_TIMER_FALLBACK)

#define NAMELEN 32
#define PTHREAD_FIRST_THREAD_ID (1)
#define PTHREAD_BARRIER_FLAG (1UL << 31)

typedef struct {
    struct MinNode node;
    struct Task *task;
    UBYTE sigbit;
} CondWaiter;

typedef struct {
    void (*destructor)(void *);
    BOOL used;
} TLSKey;

typedef struct {
    struct MinNode node;
    void (*routine)(void *);
    void *arg;
} CleanupHandler;

typedef struct {
    void *(*start)(void *);
    void *arg;
    struct Process *parent;
    int status;
    struct Process *task;
    void *ret;
    jmp_buf jmp;
    pthread_attr_t attr;
    void *tlsvalues[PTHREAD_KEYS_MAX];
    struct MinList cleanup;
    int cancelstate;
    int canceltype;
    int canceled;
    int detached;
    char name[NAMELEN];
} ThreadInfo;

extern struct Library *_DOSBase;
extern struct DOSIFace *_IDOS;

extern struct SignalSemaphore thread_sem;
extern ThreadInfo threads[PTHREAD_THREADS_MAX];
extern struct SignalSemaphore tls_sem;
extern TLSKey tlskeys[PTHREAD_KEYS_MAX];
extern APTR timerMutex;
extern struct TimeRequest *timedTimerIO;
extern struct MsgPort *timedTimerPort;

int SemaphoreIsInvalid(struct SignalSemaphore *sem);
int SemaphoreIsMine(struct SignalSemaphore *sem);
int MutexIsMine(pthread_mutex_t *mutex);
ThreadInfo *GetThreadInfo(pthread_t thread);
pthread_t GetThreadId(struct Task *task);
BOOL OpenTimerDevice(struct IORequest *io, struct MsgPort *mp, struct Task *task);
void CloseTimerDevice(struct IORequest *io);

// Private functions
void _pthread_clear_threadinfo(ThreadInfo *inf);
int _pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr, BOOL staticinit);
int _pthread_obtain_sema_timed(struct SignalSemaphore *sema, const struct timespec *abstime, int shared);
int _pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime, BOOL relative);
int _pthread_cond_broadcast(pthread_cond_t *cond, BOOL onlyfirst);

extern int _pthread_concur;

#endif