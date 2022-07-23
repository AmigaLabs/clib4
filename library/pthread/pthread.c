/*
  $Id: pthread.c,v 1.00 2022-07-18 12:09:49 clib2devs Exp $

  Copyright (C) 2014 Szilard Biro
  Copyright (C) 2018 Harry Sintonen
  Copyright (C) 2019 Stefan "Bebbo" Franke - AmigaOS 3 port

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "common.h"
#include "pthread.h"

#define GetNodeName(node) ((struct Node *)node)->ln_Name

#define ForeachNode(l,n) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
    ((struct Node *)(n))->ln_Succ; \
    n=(void *)(((struct Node *)(n))->ln_Succ))

#define ForeachNodeSafe(l,n,n2) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
        (n2=(void *)((struct Node *)(n))->ln_Succ); \
        n=(void *)n2)

int __pthread_init_func(void);
void __pthread_exit_func(void);
void __attribute__((constructor, used)) __pthread_init();
void __attribute__((destructor, used)) __pthread_exit();

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
    struct Task *parent;
    int status;
    struct Task *task;
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

static ThreadInfo threads[PTHREAD_THREADS_MAX];
static struct SignalSemaphore thread_sem;
static TLSKey tlskeys[PTHREAD_KEYS_MAX];
static struct SignalSemaphore tls_sem;

//
// Helper functions
//

static int SemaphoreIsInvalid(struct SignalSemaphore *sem) {
    return (!sem || sem->ss_Link.ln_Type != NT_SIGNALSEM || sem->ss_WaitQueue.mlh_Tail != NULL);
}

static int SemaphoreIsMine(struct SignalSemaphore *sem) {
    struct Task *me;

    me = FindTask(NULL);

    return (sem && sem->ss_NestCount > 0 && sem->ss_Owner == me);
}

static ThreadInfo *GetThreadInfo(pthread_t thread) {
    // TODO: more robust error handling?
    if (thread < PTHREAD_THREADS_MAX)
        return &threads[thread];

    return NULL;
}

static pthread_t GetThreadId(struct Task *task) {
    pthread_t i;

    // 0 is main task, First thread id will be 1 so that it is different than default value of pthread_t
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        if (threads[i].task == task)
            break;
    }

    return i;
}

static BOOL OpenTimerDevice(struct IORequest *io, struct MsgPort *mp, struct Task *task) {
    BYTE signal;

    // prepare MsgPort
    mp->mp_Node.ln_Type = NT_MSGPORT;
    mp->mp_Node.ln_Pri = 0;
    mp->mp_Node.ln_Name = NULL;
    mp->mp_Flags = PA_SIGNAL;
    mp->mp_SigTask = task;
    signal = AllocSignal(-1);
    if (signal == -1) {
        signal = SIGB_TIMER_FALLBACK;
        SetSignal(SIGF_TIMER_FALLBACK, 0);
    }
    mp->mp_SigBit = signal;
    NewList(&mp->mp_MsgList);

    // prepare IORequest
    io->io_Message.mn_Node.ln_Type = NT_MESSAGE;
    io->io_Message.mn_Node.ln_Pri = 0;
    io->io_Message.mn_Node.ln_Name = NULL;
    io->io_Message.mn_ReplyPort = mp;
    io->io_Message.mn_Length = sizeof(struct TimeRequest);

    // open timer.device
    return !OpenDevice((STRPTR) TIMERNAME, UNIT_MICROHZ, io, 0);
}

static void CloseTimerDevice(struct IORequest *io) {
    struct MsgPort *mp;

    if (!CheckIO(io)) {
        AbortIO(io);
        WaitIO(io);
    }

    if (io->io_Device != NULL)
        CloseDevice(io);

    mp = io->io_Message.mn_ReplyPort;
    if (mp->mp_SigBit != SIGB_TIMER_FALLBACK)
        FreeSignal(mp->mp_SigBit);
}

//
// Thread specific data functions
//
int
pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
    TLSKey *tls;
    int i;

    if (key == NULL)
        return EINVAL;

    ObtainSemaphore(&tls_sem);

    for (i = 0; i < PTHREAD_KEYS_MAX; i++) {
        if (tlskeys[i].used == FALSE)
            break;
    }

    if (i == PTHREAD_KEYS_MAX) {
        ReleaseSemaphore(&tls_sem);
        return EAGAIN;
    }

    tls = &tlskeys[i];
    tls->used = TRUE;
    tls->destructor = destructor;

    ReleaseSemaphore(&tls_sem);

    *key = i;

    return 0;
}

int pthread_key_delete(pthread_key_t key) {
    TLSKey *tls;

    if (key >= PTHREAD_KEYS_MAX)
        return EINVAL;

    tls = &tlskeys[key];

    ObtainSemaphore(&tls_sem);

    if (tls->used == FALSE) {
        ReleaseSemaphore(&tls_sem);
        return EINVAL;
    }

    tls->used = FALSE;
    tls->destructor = NULL;

    ReleaseSemaphore(&tls_sem);

    return 0;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
    pthread_t thread;
    ThreadInfo *inf;
    TLSKey *tls;

    if (key >= PTHREAD_KEYS_MAX)
        return EINVAL;

    thread = pthread_self();
    tls = &tlskeys[key];

    ObtainSemaphoreShared(&tls_sem);

    if (tls->used == FALSE) {
        ReleaseSemaphore(&tls_sem);
        return EINVAL;
    }

    ReleaseSemaphore(&tls_sem);

    inf = GetThreadInfo(thread);
    inf->tlsvalues[key] = (void *) value;

    return 0;
}

void *pthread_getspecific(pthread_key_t key) {
    pthread_t thread;
    ThreadInfo *inf;
    void *value = NULL;

    if (key >= PTHREAD_KEYS_MAX)
        return NULL;

    thread = pthread_self();
    inf = GetThreadInfo(thread);
    value = inf->tlsvalues[key];

    return value;
}

//
// Mutex attribute functions
//

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    attr->kind = PTHREAD_MUTEX_DEFAULT;

    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_mutexattr_t));

    return 0;
}

int pthread_mutexattr_gettype(pthread_mutexattr_t *attr, int *kind) {
    if (attr == NULL)
        return EINVAL;

    if (kind)
        *kind = attr->kind;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind) {
    if (attr == NULL || !(kind >= PTHREAD_MUTEX_NORMAL && kind <= PTHREAD_MUTEX_ERRORCHECK))
        return EINVAL;

    attr->kind = kind;

    return 0;
}

//
// Mutex functions
//

static int _pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr, BOOL staticinit) {
    if (mutex == NULL)
        return EINVAL;

    if (attr)
        mutex->kind = attr->kind;
    else if (!staticinit)
        mutex->kind = PTHREAD_MUTEX_DEFAULT;
    InitSemaphore(&mutex->semaphore);
    mutex->incond = 0;

    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    return _pthread_mutex_init(mutex, attr, FALSE);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    if (mutex == NULL)
        return EINVAL;

    // probably a statically allocated mutex
    if (SemaphoreIsInvalid(&mutex->semaphore))
        return 0;

    if (/*mutex->incond ||*/ AttemptSemaphore(&mutex->semaphore) == FALSE)
        return EBUSY;

    if (mutex->incond) {
        ReleaseSemaphore(&mutex->semaphore);
        return EBUSY;
    }

    ReleaseSemaphore(&mutex->semaphore);
    memset(mutex, 0, sizeof(pthread_mutex_t));

    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    struct SignalSemaphore *sem;

    if (mutex == NULL)
        return EINVAL;

    sem = &mutex->semaphore;

    // initialize static mutexes
    if (SemaphoreIsInvalid(sem))
        _pthread_mutex_init(mutex, NULL, TRUE);

    // normal mutexes would simply deadlock here
    if (mutex->kind == PTHREAD_MUTEX_ERRORCHECK && SemaphoreIsMine(sem))
        return EDEADLK;

    ObtainSemaphore(sem);

#if 0 // let's just deadlock here, to be compatible with normal mutexes on other platforms
    if (mutex->kind == PTHREAD_MUTEX_NORMAL && sem->ss_NestCount > 1) {
        // should have blocked - fix this
        ReleaseSemaphore(sem);
        return EDEADLK;
    }
#endif

    return 0;
}

static int _obtain_sema_timed(struct SignalSemaphore *sema, const struct timespec *abstime, int shared) {
    struct MsgPort msgport;
    struct SemaphoreMessage msg;
    struct TimeRequest timerio;
    struct Task *task;
    struct Message *m1, *m2;

    task = FindTask(NULL);

    if (!OpenTimerDevice((struct IORequest *) &timerio, &msgport, task)) {
        CloseTimerDevice((struct IORequest *) &timerio);
        return EINVAL;
    }

    timerio.Request.io_Command = TR_ADDREQUEST;
    timerio.Request.io_Flags = 0;
    TIMESPEC_TO_TIMEVAL(&timerio.Time, abstime);
    //if (!relative)
    {
        struct TimeVal starttime;
        // absolute time has to be converted to relative
        // GetSysTime can't be used due to the timezone offset in abstime
        gettimeofday((struct timeval *)&starttime, NULL);
        timersub(&timerio.Time, &starttime, &timerio.Time);
        if (!timerisset(&timerio.Time)) {
            CloseTimerDevice((struct IORequest *) &timerio);
            return ETIMEDOUT;
        }
    }
    SendIO((struct IORequest *) &timerio);

    msg.ssm_Message.mn_Node.ln_Type = NT_MESSAGE;
    msg.ssm_Message.mn_Node.ln_Name = (char *) shared;
    msg.ssm_Message.mn_ReplyPort = &msgport;
    Procure(sema, &msg);

    WaitPort(&msgport);
    m1 = GetMsg(&msgport);
    m2 = GetMsg(&msgport);
    if (m1 == &timerio.Request.io_Message || m2 == &timerio.Request.io_Message)
        Vacate(sema, &msg);

    CloseTimerDevice((struct IORequest *) &timerio);

    if (msg.ssm_Semaphore == NULL)
        return ETIMEDOUT;

    return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    int result;

    if (mutex == NULL)
        return EINVAL;

    if (abstime == NULL)
        return pthread_mutex_lock(mutex);
    else if (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000)
        return EINVAL;

    result = pthread_mutex_trylock(mutex);
    if (result != 0) {
        // pthread_mutex_trylock returns EBUSY when a deadlock would occur
        if (result != EBUSY)
            return result;
        else if (mutex->kind != PTHREAD_MUTEX_RECURSIVE && SemaphoreIsMine(&mutex->semaphore))
            return EDEADLK;
    }

    return _obtain_sema_timed(&mutex->semaphore, abstime, SM_EXCLUSIVE);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    ULONG ret;

    if (mutex == NULL)
        return EINVAL;

    // initialize static mutexes
    if (SemaphoreIsInvalid(&mutex->semaphore))
        _pthread_mutex_init(mutex, NULL, TRUE);

    if (mutex->kind != PTHREAD_MUTEX_RECURSIVE && SemaphoreIsMine(&mutex->semaphore))
        return EBUSY;

    ret = AttemptSemaphore(&mutex->semaphore);

    return (ret == TRUE) ? 0 : EBUSY;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    if (mutex == NULL)
        return EINVAL;

    // initialize static mutexes
    if (SemaphoreIsInvalid(&mutex->semaphore))
        _pthread_mutex_init(mutex, NULL, TRUE);

    if (mutex->kind != PTHREAD_MUTEX_NORMAL && !SemaphoreIsMine(&mutex->semaphore))
        return EPERM;

    ReleaseSemaphore(&mutex->semaphore);

    return 0;
}

//
// Condition variable attribute functions
//

int pthread_condattr_init(pthread_condattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_condattr_t));

    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_condattr_t));

    return 0;
}

//
// Condition variable functions
//

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
    (void) attr;
    if (cond == NULL)
        return EINVAL;

    InitSemaphore(&cond->semaphore);
    NewMinList(&cond->waiters);

    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
    if (cond == NULL)
        return EINVAL;

    // probably a statically allocated condition
    if (SemaphoreIsInvalid(&cond->semaphore))
        return 0;

    if (AttemptSemaphore(&cond->semaphore) == FALSE)
        return EBUSY;

    if (!IsMinListEmpty(&cond->waiters)) {
        ReleaseSemaphore(&cond->semaphore);
        return EBUSY;
    }

    ReleaseSemaphore(&cond->semaphore);
    memset(cond, 0, sizeof(pthread_cond_t));

    return 0;
}

static int
_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime, BOOL relative) {
    CondWaiter waiter;
    BYTE signal;
    ULONG sigs = SIGBREAKF_CTRL_C;
    struct MsgPort timermp;
    struct TimeRequest timerio;
    struct Task *task;

    if (cond == NULL || mutex == NULL)
        return EINVAL;

    // initialize static conditions
    if (SemaphoreIsInvalid(&cond->semaphore))
        pthread_cond_init(cond, NULL);

    task = FindTask(NULL);

    if (abstime) {
        // open timer.device
        if (!OpenTimerDevice((struct IORequest *) &timerio, &timermp, task)) {
            CloseTimerDevice((struct IORequest *) &timerio);
            return EINVAL;
        }

        // prepare the device command and send it
        timerio.Request.io_Command = TR_ADDREQUEST;
        timerio.Request.io_Flags = 0;
        TIMESPEC_TO_TIMEVAL(&timerio.Time, abstime);
        if (!relative) {
            struct TimeVal starttime;
            // absolute time has to be converted to relative
            // GetSysTime can't be used due to the timezone offset in abstime
            gettimeofday((struct timeval *)&starttime, NULL);
            timersub(&timerio.Time, &starttime, &timerio.Time);
            if (!timerisset(&timerio.Time)) {
                CloseTimerDevice((struct IORequest *) &timerio);
                return ETIMEDOUT;
            }
        }
        sigs |= (1 << timermp.mp_SigBit);
        SendIO((struct IORequest *) &timerio);
    }

    // prepare a waiter node
    waiter.task = task;
    signal = AllocSignal(-1);
    if (signal == -1) {
        signal = SIGB_COND_FALLBACK;
        SetSignal(SIGF_COND_FALLBACK, 0);
    }
    waiter.sigbit = signal;
    sigs |= 1 << waiter.sigbit;

    // add it to the end of the list
    ObtainSemaphore(&cond->semaphore);
    AddTail((struct List *) &cond->waiters, (struct Node *) &waiter);
    ReleaseSemaphore(&cond->semaphore);

    // wait for the condition to be signalled or the timeout
    mutex->incond++;
    pthread_mutex_unlock(mutex);
    sigs = Wait(sigs);
    pthread_mutex_lock(mutex);
    mutex->incond--;

    // remove the node from the list
    ObtainSemaphore(&cond->semaphore);
    Remove((struct Node *) &waiter);
    ReleaseSemaphore(&cond->semaphore);

    if (waiter.sigbit != SIGB_COND_FALLBACK)
        FreeSignal(waiter.sigbit);

    if (abstime) {
        // clean up the TimeRequest
        CloseTimerDevice((struct IORequest *) &timerio);

        // did we timeout?
        if (sigs & (1 << timermp.mp_SigBit))
            return ETIMEDOUT;
        else if (sigs & SIGBREAKF_CTRL_C)
            pthread_testcancel();
    } else {
        if (sigs & SIGBREAKF_CTRL_C)
            pthread_testcancel();
    }

    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
    return _pthread_cond_timedwait(cond, mutex, abstime, FALSE);
}

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime) {
    return _pthread_cond_timedwait(cond, mutex, reltime, TRUE);
}

int
pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    return _pthread_cond_timedwait(cond, mutex, NULL, FALSE);
}

static int _pthread_cond_broadcast(pthread_cond_t *cond, BOOL onlyfirst) {
    CondWaiter *waiter;

    if (cond == NULL)
        return EINVAL;

    // initialize static conditions
    if (SemaphoreIsInvalid(&cond->semaphore))
        pthread_cond_init(cond, NULL);

    // signal the waiting threads
    ObtainSemaphore(&cond->semaphore);
    ForeachNode(&cond->waiters, waiter) {
        Signal(waiter->task, 1 << waiter->sigbit);
        if (onlyfirst) break;
    }
    ReleaseSemaphore(&cond->semaphore);

    return 0;
}

int
pthread_cond_signal(pthread_cond_t *cond) {
    return _pthread_cond_broadcast(cond, TRUE);
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
    return _pthread_cond_broadcast(cond, FALSE);
}

//
// Barrier functions
//

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count) {
    (void) attr;
    if (barrier == NULL || count == 0)
        return EINVAL;

    barrier->curr_height = count;
    barrier->total_height = PTHREAD_BARRIER_FLAG;
    pthread_cond_init(&barrier->breeched, NULL);
    pthread_mutex_init(&barrier->lock, NULL);

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
    if (barrier == NULL)
        return EINVAL;

    if (pthread_mutex_trylock(&barrier->lock) != 0)
        return EBUSY;

    if (barrier->total_height > PTHREAD_BARRIER_FLAG) {
        pthread_mutex_unlock(&barrier->lock);
        return EBUSY;
    }

    pthread_mutex_unlock(&barrier->lock);

    if (pthread_cond_destroy(&barrier->breeched) != 0)
        return EBUSY;

    pthread_mutex_destroy(&barrier->lock);
    barrier->curr_height = barrier->total_height = 0;

    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier) {
    if (barrier == NULL)
        return EINVAL;

    pthread_mutex_lock(&barrier->lock);

    // wait until everyone exits the barrier
    while (barrier->total_height > PTHREAD_BARRIER_FLAG)
        pthread_cond_wait(&barrier->breeched, &barrier->lock);

    // are we the first to enter?
    if (barrier->total_height == PTHREAD_BARRIER_FLAG) barrier->total_height = 0;

    barrier->total_height++;

    if (barrier->total_height == barrier->curr_height) {
        barrier->total_height += PTHREAD_BARRIER_FLAG - 1;
        pthread_cond_broadcast(&barrier->breeched);

        pthread_mutex_unlock(&barrier->lock);

        return PTHREAD_BARRIER_SERIAL_THREAD;
    } else {
        // wait until enough threads enter the barrier
        while (barrier->total_height < PTHREAD_BARRIER_FLAG)
            pthread_cond_wait(&barrier->breeched, &barrier->lock);

        barrier->total_height--;

        // get entering threads to wake up
        if (barrier->total_height == PTHREAD_BARRIER_FLAG)
            pthread_cond_broadcast(&barrier->breeched);

        pthread_mutex_unlock(&barrier->lock);

        return 0;
    }
}

//
// Read-write lock attribute functions
//

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_rwlockattr_t));

    return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_rwlockattr_t));

    return 0;
}

//
// Read-write lock functions
//


int pthread_rwlock_init(pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr) {
    (void) (attr);

    if (lock == NULL)
        return EINVAL;

    InitSemaphore(&lock->semaphore);

    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    // probably a statically allocated rwlock
    if (SemaphoreIsInvalid(&lock->semaphore))
        return 0;

    if (AttemptSemaphore(&lock->semaphore) == FALSE)
        return EBUSY;

    ReleaseSemaphore(&lock->semaphore);
    memset(lock, 0, sizeof(pthread_rwlock_t));

    return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *lock) {
    ULONG ret;

    if (lock == NULL)
        return EINVAL;

    // initialize static rwlocks
    if (SemaphoreIsInvalid(&lock->semaphore))
        pthread_rwlock_init(lock, NULL);

    ret = AttemptSemaphoreShared(&lock->semaphore);

    return (ret == TRUE) ? 0 : EBUSY;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *lock) {
    ULONG ret;

    if (lock == NULL)
        return EINVAL;

    // initialize static rwlocks
    if (SemaphoreIsInvalid(&lock->semaphore))
        pthread_rwlock_init(lock, NULL);

    ret = AttemptSemaphore(&lock->semaphore);

    return (ret == TRUE) ? 0 : EBUSY;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    pthread_testcancel();

    // initialize static rwlocks
    if (SemaphoreIsInvalid(&lock->semaphore))
        pthread_rwlock_init(lock, NULL);

    // "Results are undefined if the calling thread holds a write lock on rwlock at the time the call is made."
    /*
    // we might already have a write lock
    if (SemaphoreIsMine(&lock->semaphore))
        return EDEADLK;
    */

    ObtainSemaphoreShared(&lock->semaphore);

    return 0;
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *lock, const struct timespec *abstime) {
    int result;

    if (lock == NULL)
        return EINVAL;

    if (abstime == NULL)
        return pthread_rwlock_rdlock(lock);
    else if (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000)
        return EINVAL;

    pthread_testcancel();

    result = pthread_rwlock_tryrdlock(lock);
    if (result != EBUSY)
        return result;

    return _obtain_sema_timed(&lock->semaphore, abstime, SM_SHARED);
}

int pthread_rwlock_wrlock(pthread_rwlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    pthread_testcancel();

    // initialize static rwlocks
    if (SemaphoreIsInvalid(&lock->semaphore))
        pthread_rwlock_init(lock, NULL);

    if (SemaphoreIsMine(&lock->semaphore))
        return EDEADLK;

    ObtainSemaphore(&lock->semaphore);

    return 0;
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *lock, const struct timespec *abstime) {
    int result;

    if (lock == NULL)
        return EINVAL;

    if (abstime == NULL)
        return pthread_rwlock_wrlock(lock);
    else if (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000)
        return EINVAL;

    pthread_testcancel();

    result = pthread_rwlock_trywrlock(lock);
    if (result != EBUSY)
        return result;

    return _obtain_sema_timed(&lock->semaphore, abstime, SM_EXCLUSIVE);
}

int pthread_rwlock_unlock(pthread_rwlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    // initialize static rwlocks
    if (SemaphoreIsInvalid(&lock->semaphore))
        pthread_rwlock_init(lock, NULL);

    //if (!SemaphoreIsMine(&lock->semaphore))
    // if no one has obtained the semaphore don't unlock the rwlock
    // this can be a leap of faith because we don't maintain a separate list of readers
    if (lock->semaphore.ss_NestCount < 1)
        return EPERM;

    ReleaseSemaphore(&lock->semaphore);

    return 0;
}

//
// Spinlock functions
//

int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
    (void) (pshared);

    if (lock == NULL)
        return EINVAL;

    *lock = 0;

    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock) {
    (void) (lock);

    return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    unsigned int cnt = 0;
    while (__sync_lock_test_and_set((int *) lock, 1)) {
        asm volatile("":: : "memory");
        if ((cnt++ & 255) == 0)
            sched_yield();
    }

    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    if (__sync_lock_test_and_set((int *) lock, 1))
        return EBUSY;

    return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
    if (lock == NULL)
        return EINVAL;

    __sync_lock_release((int *) lock);

    return 0;
}

//
// Thread attribute functions
//

int pthread_attr_init(pthread_attr_t *attr) {
    struct Task *task;

    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_attr_t));
    // inherit the priority and stack size of the parent thread

    task = FindTask(NULL);

    attr->param.sched_priority = task->tc_Node.ln_Pri;
    attr->stacksize = (UBYTE *) task->tc_SPUpper - (UBYTE *) task->tc_SPLower;

    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
    if (attr == NULL)
        return EINVAL;

    memset(attr, 0, sizeof(pthread_attr_t));

    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
    if (attr == NULL)
        return EINVAL;

    if (detachstate != NULL)
        *detachstate = attr->detachstate;

    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    if (attr == NULL || (detachstate != PTHREAD_CREATE_JOINABLE && detachstate != PTHREAD_CREATE_DETACHED))
        return EINVAL;

    attr->detachstate = detachstate;

    return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    if (attr == NULL)
        return EINVAL;

    if (stackaddr != NULL)
        *stackaddr = attr->stackaddr;

    if (stacksize != NULL)
        *stacksize = attr->stacksize;

    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    if (attr == NULL || (stackaddr != NULL && stacksize == 0))
        return EINVAL;

    attr->stackaddr = stackaddr;
    attr->stacksize = stacksize;

    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
    return pthread_attr_getstack(attr, NULL, stacksize);
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
    return pthread_attr_setstack(attr, NULL, stacksize);
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
    if (attr == NULL)
        return EINVAL;

    if (param != NULL)
        *param = attr->param;

    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
    if (attr == NULL || param == NULL)
        return EINVAL;

    attr->param = *param;

    return 0;
}

//
// Thread functions
//

static void StarterFunc() {
    ThreadInfo *inf;
    int foundkey = TRUE;
    struct StackSwapStruct stack;
    volatile BOOL stackSwapped = FALSE;

    SHOWSTRING("StarterFunc");
    struct Process *startedTask = (struct Process *) FindTask(NULL);
    inf = (ThreadInfo *) startedTask->pr_EntryData;

    // we have to set the priority here to avoid race conditions
    SetTaskPri(FindTask(NULL), inf->attr.param.sched_priority);

    // custom stack requires special handling
    if (inf->attr.stackaddr != NULL && inf->attr.stacksize > 0) {
        stack.stk_Lower = inf->attr.stackaddr;
        stack.stk_Upper = (ULONG)((APTR) stack.stk_Lower) + inf->attr.stacksize;
        stack.stk_Pointer = (APTR) stack.stk_Upper;

        StackSwap(&stack);
        stackSwapped = TRUE;
    }

    // set a jump point for pthread_exit
    if (!setjmp(inf->jmp)) {
        Printf("Mark thread %s as RUNNING\n\n\n", inf->name);
        inf->status = THREAD_STATE_RUNNING;
        inf->ret = inf->start(inf->arg);
        Printf("\n\n\nExit from start on thread %s\n", inf->name);
    }
    else {
        Printf("SETJMP on %s failed\n", inf->name);
    }

    // destroy all non-NULL TLS key values
    // since the destructors can set the keys themselves, we have to do multiple iterations
    Printf("Exit from setjmp on thread %s\n", inf->name);
    ObtainSemaphoreShared(&tls_sem);
    for (int j = 0; foundkey && j < PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
        foundkey = FALSE;
        for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
            if (tlskeys[i].used && tlskeys[i].destructor && inf->tlsvalues[i]) {
                void *oldvalue = inf->tlsvalues[i];
                inf->tlsvalues[i] = NULL;
                tlskeys[i].destructor(oldvalue);
                foundkey = TRUE;
            }
        }
    }
    ReleaseSemaphore(&tls_sem);

    if (stackSwapped)
        StackSwap(&stack);

    if (inf->status == THREAD_STATE_RUNNING) {
        ObtainSemaphore(&thread_sem);

        Printf("Starting termination of %s - Signal parent %p\n", inf->name, inf->parent);
        Printf("Mark thread %s as TERMINATED\n", inf->name);
        inf->status = THREAD_STATE_TERMINATED;
        if (!inf->detached) {
            // tell the parent thread that we are done
            Forbid();
            Signal(inf->parent, SIGF_PARENT);
        } else {
            // no one is waiting for us, do the clean up
            memset(inf, 0, sizeof(ThreadInfo));
            Printf("Mark thread %s as IDLE\n", inf->name);
            inf->status = THREAD_STATE_IDLE;
        }

        ReleaseSemaphore(&thread_sem);
    }
    else
        Printf("Thread was not running\n");
    Printf("Exit StarterFunc\n");
}

int
pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg) {
    ThreadInfo *inf;
    char name[NAMELEN] = {0};
    size_t oldlen;
    pthread_t threadnew;
    struct Task *thisTask = FindTask(NULL);

    if (thread == NULL || start == NULL)
        return EINVAL;

    ObtainSemaphore(&thread_sem);

    // grab an empty thread slot
    threadnew = GetThreadId(NULL);
    if (threadnew == PTHREAD_THREADS_MAX) {
        ReleaseSemaphore(&thread_sem);
        return EAGAIN;
    }

    // prepare the ThreadInfo structure
    inf = GetThreadInfo(threadnew);
    memset(inf, 0, sizeof(ThreadInfo));

    inf->status = THREAD_STATE_IDLE;
    inf->start = start;
    inf->arg = arg;
    inf->parent = thisTask;
    if (attr)
        inf->attr = *attr;
    else
        pthread_attr_init(&inf->attr);
    NewMinList(&inf->cleanup);
    inf->cancelstate = PTHREAD_CANCEL_ENABLE;
    inf->canceltype = PTHREAD_CANCEL_DEFERRED;
    inf->detached = inf->attr.detachstate == PTHREAD_CREATE_DETACHED;

    /* Ceck minimum stack size */
    int minStack = PTHREAD_STACK_MIN;
    int currentStack = (uint32) thisTask->tc_SPUpper - (uint32) thisTask->tc_SPLower;
    if (currentStack > minStack)
        currentStack = minStack;

    if (inf->attr.stacksize < minStack)
        inf->attr.stacksize = minStack;

    // let's trick CreateNewProc into allocating a larger buffer for the name
    snprintf(name, sizeof(name), "pthread id #%d", threadnew);
    oldlen = strlen(name);
    memset(name + oldlen, ' ', sizeof(name) - oldlen - 1);
    name[sizeof(name) - 1] = '\0';
    strncpy(inf->name, name, NAMELEN);

    BPTR fileIn  = Open("CONSOLE:", MODE_OLDFILE);
    BPTR fileOut = Open("CONSOLE:", MODE_OLDFILE);

    // start the child thread
    inf->task = (struct Task *) CreateNewProcTags(
            NP_Entry,                StarterFunc,
            inf->attr.stacksize ? TAG_IGNORE : NP_StackSize, inf->attr.stacksize,
            NP_Input,			     fileIn,
            NP_CloseInput,		     TRUE,
            NP_Output,			     fileOut,
            NP_CloseOutput,		     TRUE,
            NP_EntryData,            inf,
            NP_Name,                 name,
            NP_Cli,				     TRUE,
            TAG_DONE);

    ReleaseSemaphore(&thread_sem);

    if (0 == inf->task) {
        inf->parent = NULL;
        Close(fileIn);
        Close(fileOut);
        return EAGAIN;
    }
    Printf("\nCreation of thread %s\n", inf->name);

    *thread = threadnew;

    return 0;
}

int pthread_detach(pthread_t thread) {
    ThreadInfo *inf;

    inf = GetThreadInfo(thread);

    if (inf == NULL || inf->task == NULL)
        return ESRCH;

    if (inf->detached)
        return EINVAL;

    inf->detached = TRUE;

    return 0;
}

int pthread_join(pthread_t thread, void **value_ptr) {
    ThreadInfo *inf;
    struct Task *task;
    SHOWMSG("***************** pthread_join");
    inf = GetThreadInfo(thread);
    SHOWPOINTER(inf);
    SHOWSTRING(inf->name);
    Printf("\npthread_join\n");

    if (inf == NULL || inf->parent == NULL)
        return ESRCH;

    if (inf->detached)
        return EINVAL;

    task = FindTask(NULL);
    if (inf->task == task) {
        return EDEADLK;
    }

    pthread_testcancel();
    SHOWVALUE(inf->status);
    while (inf->status != THREAD_STATE_TERMINATED) {
        Printf("wait on thread %s\n", inf->name);
        Wait(SIGF_PARENT);
    }
    Printf("wait done\n");

    if (value_ptr)
        *value_ptr = inf->ret;

    ObtainSemaphore(&thread_sem);
    Printf("Mark thread %s as IDLE\n", inf->name);
    memset(inf, 0, sizeof(ThreadInfo));
    inf->status = THREAD_STATE_IDLE;
    ReleaseSemaphore(&thread_sem);
    Printf("pthread_join exit\n\n");

    return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2) {
    return (t1 == t2);
}

pthread_t pthread_self(void) {
    struct Task *task;
    pthread_t thread;

    task = FindTask(NULL);

    thread = GetThreadId(task);

    if (thread == PTHREAD_THREADS_MAX)
        return 0;

    return thread;
}

int pthread_cancel(pthread_t thread) {
    ThreadInfo *inf;

    inf = GetThreadInfo(thread);

    if (inf == NULL || inf->parent == NULL || inf->canceled == TRUE)
        return ESRCH;

    inf->canceled = TRUE;

    // we might have to cancel the thread immediately
    if (inf->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS && inf->cancelstate == PTHREAD_CANCEL_ENABLE) {
        struct Task *task;

        task = FindTask(NULL);

        if (inf->task == task)
            pthread_testcancel(); // cancel ourselves
        else
            Signal(inf->task, SIGBREAKF_CTRL_C); // trigger the exception handler
    } else {
        // for the timed waits
        Signal(inf->task, SIGBREAKF_CTRL_C);
    }

    return 0;
}

int pthread_setcancelstate(int state, int *oldstate) {
    pthread_t thread;
    ThreadInfo *inf;

    if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)
        return EINVAL;

    thread = pthread_self();
    inf = GetThreadInfo(thread);

    if (oldstate)
        *oldstate = inf->cancelstate;

    inf->cancelstate = state;

    return 0;
}

int pthread_setcanceltype(int type, int *oldtype) {
    pthread_t thread;
    ThreadInfo *inf;

    if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS)
        return EINVAL;

    thread = pthread_self();
    inf = GetThreadInfo(thread);

    if (oldtype)
        *oldtype = inf->canceltype;

    inf->canceltype = type;

    return 0;
}

void pthread_testcancel(void) {
    pthread_t thread;
    ThreadInfo *inf;

    thread = pthread_self();
    inf = GetThreadInfo(thread);

    if (inf->canceled && (inf->cancelstate == PTHREAD_CANCEL_ENABLE))
        pthread_exit(PTHREAD_CANCELED);

    SetSignal(SIGBREAKF_CTRL_C, 0);
}

void
pthread_exit(void *value_ptr) {
    pthread_t thread;
    ThreadInfo *inf;
    CleanupHandler *handler;

    SHOWMSG("***************** pthread_exit");
    Printf("pthread_exit called\n");
    SHOWVALUE(value_ptr);

    thread = pthread_self();
    inf = GetThreadInfo(thread);
    if (inf != NULL) {
        inf->ret = value_ptr;

        ThreadInfo *mainThread = &threads[0];
        /* If the function is called from main thread don't execute
         * cleanup handlers.
         */
        if (inf != mainThread) {
            Printf("CALLING LONGJMP\n");
            SHOWMSG("***************** CALLING LONGJMP");
            longjmp(inf->jmp, 1);
        }
    }
    Printf("pthread_exit exit\n");
}

static void OnceCleanup(void *arg) {
    pthread_once_t *once_control;

    once_control = (pthread_once_t *) arg;
    pthread_spin_unlock(&once_control->lock);
}

int
pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
    if (once_control == NULL || init_routine == NULL)
        return EINVAL;

    if (__sync_val_compare_and_swap(&once_control->started, FALSE, TRUE)) {
        pthread_spin_lock(&once_control->lock);
        if (!once_control->done) {
            pthread_cleanup_push(OnceCleanup, once_control);
            (*init_routine)();
            pthread_cleanup_pop(0);
            once_control->done = TRUE;
        }
        pthread_spin_unlock(&once_control->lock);
    }

    return 0;
}

//
// Scheduling functions
//

int
pthread_setschedprio(pthread_t thread, int prio) {
    ThreadInfo *inf;

    if (prio < sched_get_priority_max(SCHED_NORMAL) || prio > sched_get_priority_min(SCHED_NORMAL))
        return EINVAL;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;

    SetTaskPri(inf->task, prio);

    return 0;
}

int
pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) {
    ThreadInfo *inf;
    (void) (policy);

    if (param == NULL)
        return EINVAL;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;

    SetTaskPri(inf->task, param->sched_priority);

    return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
    ThreadInfo *inf;

    if (param == NULL || policy == NULL)
        return EINVAL;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;

    param->sched_priority = inf->task->tc_Node.ln_Pri;
    *policy = SCHED_NORMAL;

    return 0;
}

//
// Non-portable functions
//
int
pthread_setname_np(pthread_t thread, const char *name) {
    ThreadInfo *inf;
    char *currentname;
    size_t namelen;

    if (name == NULL)
        return ERANGE;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ERANGE;

    currentname = GetNodeName(inf->task);

    if (inf->parent == NULL)
        namelen = strlen(currentname) + 1;
    else
        namelen = NAMELEN;

    if (strlen(name) + 1 > namelen)
        return ERANGE;

    strncpy(currentname, name, namelen);

    return 0;
}

int
pthread_getname_np(pthread_t thread, char *name, size_t len) {
    ThreadInfo *inf;
    char *currentname;

    if (name == NULL || len == 0)
        return ERANGE;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ERANGE;

    currentname = GetNodeName(inf->task);

    if (strlen(currentname) + 1 > len)
        return ERANGE;

    // length check passed - strcpy is ok.
    strcpy(name, currentname);

    return 0;
}

int
pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
    ThreadInfo *inf;

    if (attr == NULL)
        return EINVAL;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH; // TODO

    *attr = inf->attr;

    return 0;
}

//
// Cancellation cleanup
//

void
pthread_cleanup_push(void (*routine)(void *), void *arg) {
    pthread_t thread;
    ThreadInfo *inf;
    CleanupHandler *handler;

    SHOWMSG("***************** pthread_cleanup_push");

    if (routine == NULL)
        return;

    handler = malloc(sizeof(CleanupHandler));
    memset(handler, 0, sizeof(CleanupHandler));
    SHOWPOINTER(handler);

    if (handler == NULL)
        return;

    thread = pthread_self();
    inf = GetThreadInfo(thread);

    handler->routine = routine;
    handler->arg = arg;
    AddTail((struct List *) &inf->cleanup, (struct Node *) handler);
}

void
pthread_cleanup_pop(int execute) {
    pthread_t thread;
    ThreadInfo *inf;
    CleanupHandler *handler;

    SHOWMSG("***************** pthread_cleanup_pop");

    thread = pthread_self();
    inf = GetThreadInfo(thread);
    handler = (CleanupHandler *)RemTail((struct List *)&inf->cleanup);

    if (handler && handler->routine && execute)
        handler->routine(handler->arg);

    free(handler);
}

//
// Signalling
//

int pthread_kill(pthread_t thread, int sig) {
    Signal((struct Task *)thread, 1 << sig);
    return EINVAL;
}

//
// Constructors, destructors
//

int __pthread_init_func(void) {
    pthread_t i;

    memset(&threads, 0, sizeof(threads));
    InitSemaphore(&thread_sem);
    InitSemaphore(&tls_sem);

    // reserve ID 0 for the main thread
    ThreadInfo *inf = &threads[0];
    inf->task = FindTask(NULL);
    inf->status = THREAD_STATE_RUNNING;
    Printf("MAIN TASK %p\n", inf->task);
    NewMinList(&inf->cleanup);

    /* Mark all threads as IDLE */
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        inf->status = THREAD_STATE_IDLE;
    }

    return TRUE;
}

void __pthread_exit_func(void) {
    pthread_t i;
    ThreadInfo *inf;

    // if we don't do this we can easily end up with unloaded code being executed
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        if (inf->detached) {
            // TODO longer delay between retries?
            while (inf->task)
                Delay(1);
        } else {
            if (inf->status == THREAD_STATE_RUNNING)
                pthread_join(i, NULL);
        }
    }
}

void __attribute__((constructor, used)) __pthread_init() {
    ENTER();

    __pthread_init_func();

    LEAVE();

    //return !__pthread_init_func();
}

void __attribute__((destructor, used)) __pthread_exit() {
    ENTER();

    __pthread_exit_func();

    LEAVE();
}
