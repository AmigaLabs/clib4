#ifndef PTHREAD_H
#define PTHREAD_H

#include <errno.h>
#include <sys/types.h>
#include <stddef.h>
#include <features.h>
#include <sched.h>

__BEGIN_DECLS

typedef unsigned long pthread_t;

typedef struct {
    short Initialized;
    void *StackAddr;
    unsigned long StackSize;
    unsigned long DetachState;
    int InheritSched;
    int SchedPolicy;
    struct sched_param SchedParam;
    unsigned long reserved[10];
} pthread_attr_t;

typedef struct {
    unsigned long Initialized;
    unsigned long Run;
} pthread_once_t;

#define PTHREAD_ONCE_INIT {1, 0}

typedef unsigned long pthread_key_t;

typedef struct {
    short Initialized;
    unsigned long Type;
    unsigned long ProcessShared;
    unsigned long reserved[10];
} pthread_mutexattr_t;

typedef unsigned long pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER       0

typedef struct {
    short Initialized;
    unsigned long ProcessShared;
    unsigned long reserved[10];
} pthread_condattr_t;

typedef unsigned long pthread_cond_t;
#define PTHREAD_COND_INITIALIZER        0

typedef struct {
    short Initialized;
    unsigned long ProcessShared;
    unsigned long reserved[10];
} pthread_rwlockattr_t;

typedef unsigned long pthread_rwlock_t;
#define PTHREAD_RWLOCK_INITIALIZER      0

enum enPThreadConst {
    PTHREAD_CREATE_JOINABLE = 0,
    PTHREAD_CREATE_DETACHED,
    PTHREAD_EXPLICIT_SCHED,
    PTHREAD_INHERIT_SCHED,
    PTHREAD_SCOPE_PROCESS,
    PTHREAD_SCOPE_SYSTEM,
    PTHREAD_CANCEL_ASYNCHRONOUS,
    PTHREAD_CANCEL_ENABLE,
    PTHREAD_CANCEL_DEFERRED,
    PTHREAD_CANCEL_DISABLE,
    PTHREAD_MUTEX_ERRORCHECK,
    PTHREAD_MUTEX_NORMAL,
    PTHREAD_MUTEX_RECURSIVE,
    PTHREAD_PROCESS_SHARED,
    PTHREAD_PROCESS_PRIVATE,
    PTHREAD_STACK_MIN = 65536,
    PTHREAD_CANCELED = -1,
};

#define PTHREAD_MUTEX_DEFAULT   PTHREAD_MUTEX_NORMAL

int pthread_create(pthread_t * , const pthread_attr_t *, void *(*)(void *), void *);
void pthread_exit(void *);
pthread_t pthread_self(void);
int pthread_equal(pthread_t, pthread_t);
int pthread_join(pthread_t, void * *);
int pthread_detach(pthread_t);
int pthread_attr_destroy(pthread_attr_t *);
int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
int pthread_attr_getinheritsched(const pthread_attr_t *, int *);
int pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *);
int pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
int pthread_attr_getscope(const pthread_attr_t *, int *);
int pthread_attr_getstackaddr(const pthread_attr_t *, void **);
int pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
int pthread_attr_init(pthread_attr_t *);
int pthread_attr_setdetachstate(pthread_attr_t *, int);
int pthread_attr_setguardsize(pthread_attr_t *, size_t);
int pthread_attr_setinheritsched(pthread_attr_t *, int);
int pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *);
int pthread_attr_setschedpolicy(pthread_attr_t *, int);
int pthread_attr_setscope(pthread_attr_t *, int);
int pthread_attr_setstackaddr(pthread_attr_t *, void *);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);
int pthread_cancel(pthread_t);
void pthread_cleanup_push(void(*)(void *), void *);
void pthread_cleanup_pop(int);
int pthread_setcancelstate(int, int *);
int pthread_setcanceltype(int, int *);
void pthread_testcancel(void);
int pthread_once(pthread_once_t *, void (*)(void));
int pthread_setconcurrency(int);
int pthread_setschedparam(pthread_t, int, const struct sched_param *);
int pthread_getconcurrency(void);
int pthread_getschedparam(pthread_t, int * , struct sched_param *);
int pthread_key_create(pthread_key_t *, void (*)(void *));
int pthread_key_delete(pthread_key_t);
void *pthread_getspecific(pthread_key_t);
int pthread_setspecific(pthread_key_t, const void *);
int pthread_mutex_destroy(pthread_mutex_t *);
int pthread_mutex_getprioceiling(const pthread_mutex_t *, int *);
int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_setprioceiling(pthread_mutex_t *, int, int *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_init(pthread_mutexattr_t *);
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
int pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int pthread_cond_broadcast(pthread_cond_t *);
int pthread_cond_destroy(pthread_cond_t *);
int pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int pthread_cond_signal(pthread_cond_t *);
int pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int pthread_condattr_destroy(pthread_condattr_t *);
int pthread_condattr_getpshared(const pthread_condattr_t *, int *);
int pthread_condattr_init(pthread_condattr_t *);
int pthread_condattr_setpshared(pthread_condattr_t *, int);
int pthread_rwlock_destroy(pthread_rwlock_t *);
int pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *);
int pthread_rwlock_rdlock(pthread_rwlock_t *);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *, const struct timespec *);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *, const struct timespec *);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
int pthread_rwlock_unlock(pthread_rwlock_t *);
int pthread_rwlock_wrlock(pthread_rwlock_t *);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *, int *);
int pthread_rwlockattr_init(pthread_rwlockattr_t *);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);

__END_DECLS

#endif
